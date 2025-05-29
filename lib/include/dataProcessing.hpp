#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <numeric>
#include <cstdint>
#include <memory>
#include <cassert>

using namespace std;

namespace RANS
{
    constexpr uint32_t lowerBound = 1u << 24;
    const uint32_t prob_bits = 16;
    const uint32_t prob_scale = 1 << prob_bits;

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

    struct SymbolStats
    {
        vector<uint32_t> frequencyArray;
        vector<uint32_t> commulativeFrequency;

        void calculateFrequency(const vector<uint8_t> &inputArray);
        void calculateCummulativeFrequency();
        void normaliseFrequency(uint32_t totalTarget);

        SymbolStats() : frequencyArray(256, 0), commulativeFrequency(257, 0) {}
    };

    typedef uint32_t state;

    static void initialiseEncoderState(state *st);

    static void normaliseEncoder(state *s, vector<uint8_t>::iterator &outputBuffer, uint32_t upperBound);

    static void encoder(state *s, vector<uint8_t>::iterator &outputBuffer, uint32_t start, uint32_t frequency, uint32_t scaleBits);

    static void encoderFlush(state *s, vector<uint8_t>::iterator &outputBuffer);

    static void initialiseDecoderState(state *s, vector<uint8_t>::iterator &outputBuffer);

    static  uint32_t getCFforDecodingSymbol(state *s, uint32_t scaleBits);

    static state normaliseDecoder(state *s, vector<uint8_t>::iterator &outputBuffer);

    static void decoder(state *s, vector<uint8_t>::iterator &outputBuffer, uint32_t start, uint32_t frequency, uint32_t scaleBits);
    
    static void encodingSymbolInitialise(encoderSymbol *symb, uint32_t start, uint32_t frequency, uint32_t scaleBits)
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
    
    static void decodingSymbolInitialise(decoderSymbol *s, uint32_t start, uint32_t frequency)
    {
        if (start >= (1 << 16))
            start = (1 << 16) - 1;
        if (frequency > ((1 << 16) - start))
            frequency = ((1u << 16) - start);
        s->start = start;
        s->frequency = frequency;
    }
    
    static inline void getSymbolFromEncoder(state *s, vector<uint8_t>::iterator &outputBuffer, encoderSymbol const *sym);

    static inline void decoderWithSymbolTable(state *s, vector<uint8_t>::iterator &outputBuffer, decoderSymbol const *sym, uint32_t scaleBits);

    vector<uint8_t> populateCummulativeFreq2Symbol(const SymbolStats &stats, uint32_t prob_scale);

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
        RANS(const vector<uint8_t> &input) : inputArray(input), encodingSymbols(256), decodingSymbols(256)
        {
            stats.calculateFrequency(inputArray);
            stats.normaliseFrequency(prob_scale);

            cummulativeFreq2Symbol = populateCummulativeFreq2Symbol(stats, prob_scale);

            // Dynamically size outputBuffer based on input
            outputBuffer.resize(1 << 20); // Generous estimate for encoded size
            decodingBytes.resize(inputArray.size());

            for (int i = 0; i < 256; i++)
            {
                encodingSymbolInitialise(&encodingSymbols[i], stats.commulativeFrequency[i],
                                         stats.commulativeFrequency[i + 1] - stats.commulativeFrequency[i], prob_bits);

                decodingSymbolInitialise(&decodingSymbols[i], stats.commulativeFrequency[i],
                                         stats.commulativeFrequency[i + 1] - stats.commulativeFrequency[i]);
            }
        }

        vector<uint8_t> encode();

        vector<uint8_t> decode(vector<uint8_t> &encoded, size_t original_size);
    };
}

namespace compression
{

    class compressorDecompressor
    {
    public:
        std::pair<std::vector<uint8_t>,size_t> compress(const std::vector<float> &input);
        std::vector<float> decompress(std::vector<uint8_t> &compressed, size_t originalSize,size_t compressedSize);

    private:
        RANS::RANS *rans = nullptr;
        const static uint32_t TABLE_SIZE = 1 << 16;
        uint64_t fcm[TABLE_SIZE] = {0};
        uint64_t dfcm[TABLE_SIZE] = {0};
        uint32_t fcm_hash = 0;
        uint32_t dfcm_hash = 0;
        uint64_t last_value = 0;

        uint64_t getFcmPrediction();
        uint64_t getDfcmPrediction();
        void reset();
        void updateFcmHash(uint64_t true_value);
        void updateDfcmHash(uint64_t true_value);
        static uint8_t encodeZeroBytes(uint64_t diff);
        uint64_t toLong(const std::vector<uint8_t> &dst);
    };
}