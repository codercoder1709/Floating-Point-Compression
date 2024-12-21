#include "dataProcessing.hpp"
#include "ranslassan.cpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <memory>
using namespace std;

namespace compression
{
    uint64_t compressorDecompressor::getFcmPrediction()
    {
        return fcm[fcm_hash];
    }
    uint64_t compressorDecompressor::getDfcmPrediction()
    {
        return dfcm[dfcm_hash] + last_value;
    }


    
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





    uint8_t compressorDecompressor::encodeZeroBytes(uint64_t diff)
    {
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

    std::vector<uint8_t> compressorDecompressor::compress(const std::vector<double> &input)
    {

        std::vector<uint8_t> compressed;

        for (size_t i = 0; i < input.size(); i+=2)
        {
            uint64_t first_true_value = *reinterpret_cast<const uint64_t *>(&input[i]);

            uint64_t first_fcm_prediction = getFcmPrediction();
            uint64_t first_dfcm_difference_prediction = getDfcmPrediction();

            bool first_use_fcm = __builtin_clzll(first_true_value ^ first_fcm_prediction) >= __builtin_clzll(first_true_value ^ first_dfcm_difference_prediction);

            updateFcmHash(first_true_value);
            updateDfcmHash(first_true_value);

            cout<<fcm_hash<<" "<<dfcm_hash+last_value<<" "<<endl;

            uint64_t second_true_value = *reinterpret_cast<const uint64_t *>(&input[i + 1]);

            uint64_t second_fcm_prediction = getFcmPrediction();
            uint64_t second_dfcm_difference_prediction = getDfcmPrediction();

            bool second_use_fcm = __builtin_clzll(second_true_value ^ second_fcm_prediction) >= __builtin_clzll(second_true_value ^ second_dfcm_difference_prediction);

            updateFcmHash(second_true_value);
            updateDfcmHash(second_true_value);

            //cout<<(first_true_value ^ first_fcm_prediction)<<" "<<(first_true_value ^ first_dfcm_difference_prediction)<<" "<<(second_true_value ^ second_fcm_prediction)<<" "<<(second_true_value ^ second_dfcm_difference_prediction)<<endl;;
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

        // auto encoded = encode(compressed);
        return compressed;
    }

    std::vector<double> compressorDecompressor::decompress(std::vector<uint8_t> &compressed, size_t originalSize)
    {

        // auto fpcCpmpreesed = decode(compressed, compressed.size());
        auto fpcCpmpreesed = compressed;
        reverse(fpcCpmpreesed.begin(),fpcCpmpreesed.end());

        std::vector<double> decompressed;
        size_t bufferIndex = 0;

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
            decompressed.push_back(*reinterpret_cast<double *>(&actual));

            if ((header & 0x08) != 0)
            {
                prediction = getDfcmPrediction();
            }
            else
            {
                prediction = getFcmPrediction();
            }

            // Calculate numZeroBytes for second value
            numZeroBytes = (header & 0x07);
            if (numZeroBytes > 3) numZeroBytes++;
            
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
            decompressed.push_back(*reinterpret_cast<double *>(&actual));
        }

        return decompressed;
    }

}
// void compressorDecompressor::predictorState::reset()
// {
//     std::fill_n(fcm, TABLE_SIZE, 0);
//     std::fill_n(dfcm, TABLE_SIZE, 0);
//     fcm_hash = 0;
//     dfcm_hash = 0;
//     last_value = 0;
// }
// long dBits = Double.doubleToLongBits(doubles[i]);
//             long diff1d = predictor1.getPrediction() ^ dBits;
//             long diff2d = predictor2.getPrediction() ^ dBits;

//             boolean predictor1BetterForD = Long.numberOfLeadingZeros(diff1d) >= Long.numberOfLeadingZeros(diff2d);

//             predictor1.update(dBits);
//             predictor2.update(dBits);

//             long eBits = Double.doubleToLongBits(doubles[i + 1]);
//             long diff1e = predictor1.getPrediction() ^ eBits;
//             long diff2e = predictor2.getPrediction() ^ eBits;

//             boolean predictor1BetterForE = Long.numberOfLeadingZeros(diff1e) >= Long.numberOfLeadingZeros(diff2e);

//             predictor1.update(eBits);
//             predictor2.update(eBits);

// uint8_t code = (i % 2 == 0)
//                    ? (fpcCpmpreesed[bufferIndex] >> 4)
//                    : (fpcCpmpreesed[bufferIndex] & 0x0F);

// int leading_zero_bytes = code & 0b111;
// bool switch_predictor = (code & 0b1000);

// uint64_t remainder = 0;
// int remainder_bytes = 8 - leading_zero_bytes;
// for (int j = 0; j < remainder_bytes; ++j)
// {
//     remainder = (remainder << 8) | fpcCpmpreesed[bufferIndex + 1 + j];
// }

// uint64_t prediction = switch_predictor ? fcm[fcm_hash] : dfcm[dfcm_hash] + last_value;

// uint64_t first_true_value = prediction ^ remainder;
// decompressed.push_back(*reinterpret_cast<double *>(&first_true_value));

// bufferIndex += 1 + remainder_bytes;

// fcm[fcm_hash] = first_true_value;
// fcm_hash = ((fcm_hash << 6) ^ (first_true_value >> 48)) & (TABLE_SIZE - 1);

// if (i > 0)
// {
//     uint64_t difference = first_true_value - last_value;
//     dfcm[dfcm_hash] = difference;
//     dfcm_hash = ((dfcm_hash << 2) ^ (difference >> 40)) & (TABLE_SIZE - 1);
// }

// last_value = first_true_value;