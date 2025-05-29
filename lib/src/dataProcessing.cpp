#include "dataProcessing.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <memory>
using namespace std;

namespace RANS
{
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

    static uint32_t getCFforDecodingSymbol(state *s, uint32_t scaleBits)
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

    vector<uint8_t> RANS::encode()
    {
        initialiseEncoderState(&rans);

        ptr = outputBuffer.end();

        for (auto i = inputArray.rbegin(); i != inputArray.rend(); ++i)
        {
            getSymbolFromEncoder(&rans, ptr, &encodingSymbols[*i]);
        }

        encoderFlush(&rans, ptr);
        rans_begin = ptr;

        return vector<uint8_t>(rans_begin, outputBuffer.end());
    }

    vector<uint8_t> RANS::decode(vector<uint8_t> &encoded, size_t original_size)
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

}

namespace compression
{
    void compressorDecompressor::updateFcmHash(uint64_t true_value)
    {
        fcm[fcm_hash] = true_value;
        fcm_hash = ((fcm_hash << 6) ^ (true_value >> 48)) & (TABLE_SIZE - 1);
    }
    void compressorDecompressor::updateDfcmHash(uint64_t true_value)
    {
        dfcm[dfcm_hash] = (true_value - last_value);
        dfcm_hash = ((dfcm_hash << 2) ^ ((true_value - last_value) >> 40)) & (TABLE_SIZE - 1);
        last_value = true_value;
    }

    void compressorDecompressor::reset()
    {
        fcm_hash = 0;
        dfcm_hash = 0;
        last_value = 0;
        std::fill_n(fcm, TABLE_SIZE, 0);
        std::fill_n(dfcm, TABLE_SIZE, 0);
    }
    uint64_t compressorDecompressor::getFcmPrediction()
    {
        return fcm[fcm_hash];
    }
    uint64_t compressorDecompressor::getDfcmPrediction()
    {
        return dfcm[dfcm_hash] + last_value;
    }

    uint8_t compressorDecompressor::encodeZeroBytes(uint64_t diff)
    {
        if (diff == 0)
            return 7;
        uint8_t leadingZeroBytes = __builtin_clzll(diff) / 8;
        if (leadingZeroBytes >= 4)
            leadingZeroBytes--;
        return leadingZeroBytes;
    }

    std::vector<uint8_t> toByteArray(uint64_t diff)
    {
        uint8_t leadingZeroBytes = __builtin_clzll(diff) / 8;
        if (leadingZeroBytes >= 4)
            leadingZeroBytes--;
        if (diff == 0)
            leadingZeroBytes = 7;

        int encodedZeroBytes = leadingZeroBytes;

        if (encodedZeroBytes > 3)
            encodedZeroBytes++;

        std::vector<uint8_t> array(8 - encodedZeroBytes);

        for (size_t i = 0; i < array.size(); i++)
        {
            array[i] = static_cast<uint8_t>(diff & 0xff);
            diff >>= 8;
        }

        return array;
    }

    uint64_t compressorDecompressor::toLong(const std::vector<uint8_t> &dst)
    {
        uint64_t result = 0;
        for (size_t i = 0; i < dst.size(); ++i)
        {
            result |= static_cast<uint64_t>(dst[i]) << (i * 8);
        }
        return result;
    }

    std::pair<std::vector<uint8_t>, size_t> compressorDecompressor::compress(const std::vector<float> &input)
    {

        std::vector<uint8_t> compressed;

        for (size_t i = 0; i < input.size(); i += 2)
        {
            uint64_t first_true_value = *reinterpret_cast<const uint64_t *>(&input[i]);

            uint64_t first_fcm_prediction = getFcmPrediction();
            uint64_t first_dfcm_difference_prediction = getDfcmPrediction();

            bool first_use_fcm = __builtin_clzll(first_true_value ^ first_fcm_prediction) >= __builtin_clzll(first_true_value ^ first_dfcm_difference_prediction);

            updateFcmHash(first_true_value);
            updateDfcmHash(first_true_value);

            uint64_t second_true_value = *reinterpret_cast<const uint64_t *>(&input[i + 1]);

            uint64_t second_fcm_prediction = getFcmPrediction();
            uint64_t second_dfcm_difference_prediction = getDfcmPrediction();

            bool second_use_fcm = __builtin_clzll(second_true_value ^ second_fcm_prediction) >= __builtin_clzll(second_true_value ^ second_dfcm_difference_prediction);

            updateFcmHash(second_true_value);
            updateDfcmHash(second_true_value);

            uint8_t code = 0;
            if (first_use_fcm)
            {

                uint8_t zeroBytes = encodeZeroBytes(first_true_value ^ first_fcm_prediction);
                code |= zeroBytes << 4;
            }
            else
            {
                code |= 0x80;

                int zeroBytes = encodeZeroBytes(first_true_value ^ first_dfcm_difference_prediction);
                code |= zeroBytes << 4;
            }
            if (second_use_fcm)
            {

                int zeroBytes = encodeZeroBytes(second_true_value ^ second_fcm_prediction);
                code |= zeroBytes;
            }
            else
            {
                code |= 0x08;
                int zeroBytes = encodeZeroBytes(second_true_value ^ second_dfcm_difference_prediction);
                code |= zeroBytes;
            }
            compressed.push_back(code);

            if (first_use_fcm)
            {
                std::vector<uint8_t> byteArrayD = toByteArray(first_true_value ^ first_fcm_prediction);
                compressed.insert(compressed.end(), byteArrayD.begin(), byteArrayD.end());
            }
            else
            {
                std::vector<uint8_t> byteArrayD = toByteArray(first_true_value ^ first_dfcm_difference_prediction);
                compressed.insert(compressed.end(), byteArrayD.begin(), byteArrayD.end());
            }

            if (second_use_fcm)
            {
                std::vector<uint8_t> byteArrayE = toByteArray(second_true_value ^ second_fcm_prediction);
                compressed.insert(compressed.end(), byteArrayE.begin(), byteArrayE.end());
            }
            else
            {
                std::vector<uint8_t> byteArrayE = toByteArray(second_true_value ^ second_dfcm_difference_prediction);
                compressed.insert(compressed.end(), byteArrayE.begin(), byteArrayE.end());
            }
        }

        if (input.size() % 2 != 0)
        {
            compressed.push_back(0x00);
        }

        rans = new RANS::RANS(compressed);

        auto encoded = rans->encode();
        return {encoded, compressed.size()};
    }

    std::vector<float> compressorDecompressor::decompress(std::vector<uint8_t> &compressed, size_t originalSize, size_t compressedSize)
    {

        auto fpcCpmpreesed = rans->decode(compressed, compressedSize);

        std::vector<float> decompressed;
        size_t bufferIndex = 0;
        reset();
        for (size_t i = 0; i < originalSize; i += 2)
        {
            uint8_t header = fpcCpmpreesed[bufferIndex++];
            uint64_t prediction;

            if ((header & 0x80) != 0)
            {
                prediction = getDfcmPrediction();
            }
            else
            {
                prediction = getFcmPrediction();
            }

            int numZeroBytes = (header & 0x70) >> 4;
            if (numZeroBytes > 3)
            {
                numZeroBytes++;
            }

            size_t diffSize = 8 - numZeroBytes;
            std::vector<uint8_t> dst(diffSize);
            for (size_t j = 0; j < diffSize; ++j)
            {
                dst[j] = fpcCpmpreesed[bufferIndex++];
            }
            int64_t diff = toLong(dst);
            int64_t actual = prediction ^ diff;

            updateFcmHash(actual);
            updateDfcmHash(actual);
            decompressed.push_back(*reinterpret_cast<float *>(&actual));

            if ((header & 0x08) != 0)
            {
                prediction = getDfcmPrediction();
            }
            else
            {
                prediction = getFcmPrediction();
            }
            numZeroBytes = (header & 0x07);
            if (numZeroBytes > 3)
                numZeroBytes++;

            // Extract bytes for second diff
            diffSize = 8 - numZeroBytes;
            dst.resize(diffSize);
            for (size_t j = 0; j < diffSize; ++j)
            {
                dst[j] = fpcCpmpreesed[bufferIndex++];
            }
            diff = toLong(dst);

            // Special case for termination
            if (numZeroBytes == 7 && diff == 0)
            {
                return {};
            }

            actual = prediction ^ diff;

            // Update predictors and decode the second value
            updateFcmHash(actual);
            updateDfcmHash(actual);
            decompressed.push_back(*reinterpret_cast<float *>(&actual));
        }

        return decompressed;
    }
}