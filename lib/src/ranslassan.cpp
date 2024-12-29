#include <cstdint>
#include <vector>
#include <cassert>
#include <iostream>
using namespace std;

constexpr uint32_t lowerBound = 1u << 23;

typedef uint32_t state;

static void initialiseEncoderState(state *st)
{
    *st = lowerBound;
}

static void normaliseEncoder(state *s, vector<uint8_t>::iterator &outputBuffer, uint32_t upperBound)
{
    uint32_t x = *s;
    if (x > upperBound)
    {
        do
        {
            --outputBuffer;
            *outputBuffer = (uint8_t)(x & 0xff);
            x >>= 8;
        } while (x > upperBound);
    }
    *s = x;
}

static void encoder(state *s, vector<uint8_t>::iterator &outputBuffer, uint32_t start, uint32_t frequency, uint32_t scaleBits)
{
    const uint32_t precision = 32;
    uint32_t reciprocal = ((1ull << precision) + frequency - 1) / frequency;
    uint64_t quotient = ((uint64_t)*s * reciprocal) >> precision;
    uint32_t remainder = *s - (quotient * frequency);
    *s = (quotient << scaleBits) + remainder + start;
}

static void encoderFlush(state *s, vector<uint8_t>::iterator &outputBuffer)
{
    uint32_t x = *s;
    outputBuffer -= 4;

    for (int i = 0; i < 4; i++)
        outputBuffer[i] = (uint8_t)(x >> (i * 8));
}

static void initialiseDecoderState(state *s, vector<uint8_t>::iterator &outputBuffer)
{
    uint32_t x = 0;
    for (int i = 0; i < 4; i++)
    {
        x |= (uint32_t)outputBuffer[i] << (i * 8);
    }
    outputBuffer += 4;
    *s = x;
}

static inline uint32_t getCFforDecodingSymbol(state *s, uint32_t scaleBits)
{
    return *s & ((1u << scaleBits) - 1);
}

static state normaliseDecoder(state *s, vector<uint8_t>::iterator &outputBuffer)
{
    state x = *s;
    if (x < lowerBound)
    {
        do
        {
            x = (x << 8) | *outputBuffer;
            ++outputBuffer;
        } while (x < lowerBound);
    }

    return x;
}

static void decoder(state *s, vector<uint8_t>::iterator &outputBuffer, uint32_t start, uint32_t frequency, uint32_t scaleBits)
{
    uint32_t mask = (1u << scaleBits) - 1;
    uint32_t x = *s;

    x = frequency * (x >> scaleBits) + (x & mask) - start;
    *s = normaliseDecoder(&x, outputBuffer);
}

typedef struct
{
    uint32_t upperBound;
    uint32_t frequencyInverse;
    uint32_t bias;
    uint16_t frequencyCompliment;
    uint16_t reciprocalShift;
} encoderSymbol;

typedef struct
{
    uint16_t start;
    uint16_t frequency;
} decoderSymbol;

static inline void encodingSymbolInitialise(encoderSymbol *symb, uint32_t start, uint32_t frequency, uint32_t scaleBits)
{
    assert(scaleBits <= 16);
    assert(start <= (1u << scaleBits));
    if (frequency > ((1u << scaleBits) - start))
        frequency = ((1u << scaleBits) - start);

    symb->upperBound = ((lowerBound >> scaleBits) << 8) * frequency;
    symb->frequencyCompliment = ((1 << scaleBits) - frequency);
    if (frequency < 2)
    {
        symb->frequencyInverse = ~0u;
        symb->reciprocalShift = 0;
        symb->bias = start + (1 << scaleBits) - 1;
    }
    else
    {
        uint32_t shift = 0;
        while (frequency > (1u << shift))
            shift++;

        symb->frequencyInverse = (uint32_t)(((1ull << (shift + 31)) + frequency - 1) / frequency);
        symb->reciprocalShift = shift - 1;
        symb->bias = start;
    }
}

static inline void decodingSymbolInitialise(decoderSymbol *s, uint32_t start, uint32_t frequency)
{
    if (start >= (1 << 16))
        start = (1 << 16) - 1;
    if (frequency > ((1 << 16) - start))
        frequency = ((1u << 16) - start);
    s->start = start;
    s->frequency = frequency;
}

static inline void getSymbolFromEncoder(state *s, vector<uint8_t>::iterator &outputBuffer, encoderSymbol const *sym)
{
    if (sym->upperBound == 0)
        return;
    uint32_t x = *s;
    normaliseEncoder(&x, outputBuffer, sym->upperBound);

    uint32_t q = (uint32_t)(((uint64_t)x * sym->frequencyInverse) >> 32) >> sym->reciprocalShift;
    *s = x + sym->bias + q * sym->frequencyCompliment;
}

static inline void decoderWithSymbolTable(state *s, vector<uint8_t>::iterator &outputBuffer, decoderSymbol const *sym, uint32_t scaleBits)
{
    decoder(s, outputBuffer, sym->start, sym->frequency, scaleBits);
}

const uint32_t prob_bits = 14;
const uint32_t prob_scale = 1 << prob_bits;

struct SymbolStats
{
    vector<uint32_t> frequencyArray;
    vector<uint32_t> commulativeFrequency;

    void calculateFrequency(const vector<uint8_t> &inputArray);
    void calculateCummulativeFrequency();
    void normaliseFrequency(uint32_t totalTarget);

    SymbolStats() : frequencyArray(256, 0), commulativeFrequency(257, 0) {}
};

void SymbolStats::calculateFrequency(const vector<uint8_t> &inputArray)
{
    for (uint8_t byte : inputArray)
        frequencyArray[byte]++;
}

void SymbolStats::calculateCummulativeFrequency()
{
    commulativeFrequency[0] = 0;
    for (int i = 0; i < 256; i++)
        commulativeFrequency[i + 1] = commulativeFrequency[i] + frequencyArray[i];
}

void SymbolStats::normaliseFrequency(uint32_t totalTarget)
{
    assert(totalTarget >= 256);

    calculateCummulativeFrequency();
    uint32_t currentTotal = commulativeFrequency[256];

    for (int i = 1; i <= 256; i++)
        commulativeFrequency[i] = ((uint64_t)totalTarget * commulativeFrequency[i]) / currentTotal;

    for (int i = 0; i < 256; i++)
    {
        if (frequencyArray[i] && commulativeFrequency[i + 1] == commulativeFrequency[i])
        {
            int best_steal = -1;
            uint32_t best_freq = ~0u;

            for (int j = 0; j < 256; j++)
            {
                uint32_t freq = commulativeFrequency[j + 1] - commulativeFrequency[j];
                if (freq > 1 && freq < best_freq)
                {
                    best_freq = freq;
                    best_steal = j;
                }
            }

            assert(best_steal != -1);

            if (best_steal < i)
            {
                for (int j = best_steal + 1; j <= i; j++)
                    commulativeFrequency[j]--;
            }
            else
            {
                assert(best_steal > i);
                for (int j = i + 1; j <= best_steal; j++)
                    commulativeFrequency[j]++;
            }
        }
    }
}

vector<uint8_t> populateCummulativeFreq2Symbol(const SymbolStats &stats, uint32_t prob_scale)
{
    vector<uint8_t> cummulativeFreq2Symbol(prob_scale);
    for (int s = 0; s < 256; s++)
    {
        uint32_t start = stats.commulativeFrequency[s];
        uint32_t end = stats.commulativeFrequency[s + 1];
        std::fill(cummulativeFreq2Symbol.begin() + start, cummulativeFreq2Symbol.begin() + end, s);
    }
    return cummulativeFreq2Symbol;
}

class RANS
{
    SymbolStats stats;
    vector<uint8_t> cummulativeFreq2Symbol;
    vector<uint8_t> outputBuffer;
    vector<uint8_t> decodingBytes;
    vector<uint8_t>::iterator rans_begin;
    vector<uint8_t> inputArray;
    vector<encoderSymbol> encodingSymbols;
    vector<decoderSymbol> decodingSymbols;
    state rans;
    vector<uint8_t>::iterator ptr;

public:
    RANS(const vector<uint8_t>& input) : 
        inputArray(input), 
        encodingSymbols(256), 
        decodingSymbols(256)
    {
        stats.calculateFrequency(inputArray);
        stats.normaliseFrequency(prob_scale);

        cummulativeFreq2Symbol = populateCummulativeFreq2Symbol(stats, prob_scale);

        // Dynamically size outputBuffer based on input
        outputBuffer.resize(1<<16);  // Generous estimate for encoded size
        decodingBytes.resize(inputArray.size());

        for (int i = 0; i < 256; i++)
        {
            encodingSymbolInitialise(&encodingSymbols[i], stats.commulativeFrequency[i], 
                stats.commulativeFrequency[i + 1] - stats.commulativeFrequency[i], prob_bits);
            
            decodingSymbolInitialise(&decodingSymbols[i], stats.commulativeFrequency[i], 
                stats.commulativeFrequency[i + 1] - stats.commulativeFrequency[i]);
        }
    }

    vector<uint8_t> encode()
    {
        initialiseEncoderState(&rans);

        ptr = outputBuffer.end();

        for (auto i = inputArray.rbegin(); i != inputArray.rend(); ++i)
        {
            getSymbolFromEncoder(&rans, ptr, &encodingSymbols[*i]);
        }
        
        encoderFlush(&rans, ptr);
        rans_begin = ptr;

        // Copy the actual encoded data 
        return vector<uint8_t>(rans_begin, outputBuffer.end());
    }

    vector<uint8_t> decode(vector<uint8_t>&encoded,size_t original_size)
    {
        auto ptr = encoded.begin();
        initialiseDecoderState(&rans, ptr);

        for (size_t i = 0; i < original_size; i++)
        {
            uint32_t s = cummulativeFreq2Symbol[getCFforDecodingSymbol(&rans, prob_bits)];
            decodingBytes[i] = (uint8_t)s;
            decoderWithSymbolTable(&rans, ptr, &decodingSymbols[s], prob_bits);
        }

        return decodingBytes;
    }
};

int main() 
{
    vector<uint8_t> inputArray = {1, 2, 3, 4, 5,6,7,8,9,10,11,12,13,14,15};
    RANS obj(inputArray);
    
    vector<uint8_t> encoded = obj.encode();
    vector<uint8_t> decoded = obj.decode(encoded,inputArray.size());
    
    cout << "Encoded: ";
    for(auto byte : encoded)
        cout << static_cast<int>(byte) << " ";
    cout << "\n";
    
    cout << "Decoded: ";
    for(auto byte : decoded)
        cout << static_cast<int>(byte) << " ";
    cout << "\n";

    return 0;
}