#include "dataProcessing.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <memory>
using namespace std;

namespace compression
{
    void compressorDecompressor::predictorState::reset()
    {
        std::fill_n(fcm, TABLE_SIZE, 0);
        std::fill_n(dfcm, TABLE_SIZE, 0);
        fcm_hash = 0;
        dfcm_hash = 0;
        last_value = 0;
    }

    void compressorDecompressor::countFrequency(const std::vector<uint8_t> &data, uint32_t totalTarget)
    {
        // Clear existing frequency data
        freq_table.clear();
        cumulative_freq.clear();
        total_freq = 0;

        // Count symbol frequencies
        for (uint8_t byte : data)
            freq_table[byte]++;

        // Calculate initial cumulative frequencies
        uint32_t cumul = 0;
        for (int sym = 0; sym < 256; sym++)
        {
            if (freq_table.count(sym) > 0)
            {
                cumulative_freq[sym] = cumul;
                cumul += freq_table[sym];
            }
            else
            {
                // Ensure all symbols have a cumulative frequency entry
                cumulative_freq[sym] = cumul;
            }
        }
        total_freq = cumul;

        // Scale cumulative frequencies to target total
        if (total_freq > 0)
        {
            for (int i = 0; i < 256; i++)
            {
                // Prevent potential overflow by using 64-bit multiplication
                cumulative_freq[i] = static_cast<uint32_t>(
                    (static_cast<uint64_t>(totalTarget) * cumulative_freq[i]) / total_freq);
            }
        }

        // Ensure unique cumulative frequencies and prevent zero-width symbols
        std::vector<int> problematic_symbols;
        for (int i = 0; i < 255; i++)
        {
            if (freq_table[i] > 0 && cumulative_freq[i] == cumulative_freq[i + 1])
            {
                problematic_symbols.push_back(i);
            }
        }

        // Resolve zero-width symbols
        for (int sym : problematic_symbols)
        {
            // Find the best symbol to steal frequency from
            int best_steal = -1;
            uint32_t smallest_gap = UINT32_MAX;

            for (int j = 0; j < 256; j++)
            {
                // Skip if frequency is too small
                if (j == sym)
                    continue;

                uint32_t curr_gap = cumulative_freq[j + 1] - cumulative_freq[j];
                if (curr_gap > 1 && curr_gap < smallest_gap)
                {
                    smallest_gap = curr_gap;
                    best_steal = j;
                }
            }

            // Redistribute frequencies
            if (best_steal != -1)
            {
                if (best_steal < sym)
                {
                    // Decrease frequencies of symbols between best_steal and sym
                    for (int j = best_steal + 1; j <= sym; j++)
                    {
                        cumulative_freq[j]--;
                    }
                }
                else
                {
                    // Increase frequencies of symbols between sym and best_steal
                    for (int j = sym + 1; j <= best_steal; j++)
                    {
                        cumulative_freq[j]++;
                    }
                }
            }
        }

        // Final sanity check to ensure no zero-width symbols
        for (int i = 0; i < 255; i++)
        {
            if (freq_table[i] > 0 && cumulative_freq[i] == cumulative_freq[i + 1])
            {
                // Emergency fallback: force a small difference
                cumulative_freq[i + 1]++;
            }
        }
    }

    uint8_t compressorDecompressor::decode_symbol(uint64_t &x)
    {
        for (int sym = 0; sym < 256; sym++)
        {
            if (freq_table.count(sym) > 0)
            {
                uint32_t symFreq = freq_table[sym];
                uint32_t symCumul = cumulative_freq[sym];
                uint32_t nextCumul = cumulative_freq[sym + 1];

                if (x >= symCumul && x < nextCumul)
                {
                    return sym;
                }
            }
        }
        throw std::runtime_error("Symbol decoding failed");
    }

    std::vector<uint8_t> compressorDecompressor::encode(const std::vector<uint8_t> &data)
    {
        countFrequency(data, 1 << 16);

        state = 0;
        std::vector<uint8_t> output;

        for (auto it = data.rbegin(); it != data.rend(); ++it)
        {
            uint8_t sym = *it;
            uint32_t symFreq = freq_table[sym];
            uint32_t symCumul = cumulative_freq[sym];

            while (state >= ((1ull << RANS_NORMALIZATION_BITS) / symFreq) * total_freq)
            {
                output.push_back(state & 0xFF);
                state >>= 8;
            }

            state = ((state / symFreq) << RANS_PRECISION) + (state % symFreq) + symCumul;
        }

        uint32_t remaining_state = state;
        for (int i = 0; i < 4; ++i)
        {
            output.push_back(remaining_state & 0xFF);
            remaining_state >>= 8;
        }

        std::reverse(output.begin(), output.end());

        return output;
    }

    

    std::vector<uint8_t> compressorDecompressor::decode(std::vector<uint8_t> &encoded, size_t original_size)
    {
        std::vector<uint8_t> decoded;
        decoded.reserve(original_size);

        uint64_t state = 0;
        for (int i = 0; i < 4; ++i)
        {
            state = (state << 8) | encoded[encoded.size() - 1 - i];
        }

        while (decoded.size() < original_size)
        {
            // Extract the low-precision bits for symbol decoding
            uint64_t x = state & ((1U << RANS_PRECISION) - 1);
            uint8_t sym = decode_symbol(x);
            decoded.push_back(sym);

            // Update the state
            uint32_t symFreq = freq_table[sym];
            uint32_t symCumul = cumulative_freq[sym];
            state = (state / symFreq) * total_freq + x + symCumul;

            // Renormalization
            while (state < (1ull << RANS_NORMALIZATION_BITS))
            {
                if (!encoded.empty())
                {
                    state = (state << 8) | encoded.back();
                    encoded.pop_back();
                }
                else
                {
                    break;
                }
            }
        }

        std::reverse(decoded.begin(), decoded.end());
        return decoded;
    }

    std::vector<uint8_t> compressorDecompressor::compress(const std::vector<double> &input)
    {
        predictorState state;
        state.reset();

        std::vector<uint8_t> compressed;

        for (size_t i = 0; i < input.size(); ++i)
        {
            uint64_t true_value = *reinterpret_cast<const uint64_t *>(&input[i]);

            uint64_t fcm_prediction = state.fcm[state.fcm_hash];
            uint64_t dfcm_difference_prediction = (i > 0) ? state.dfcm[state.dfcm_hash] + state.last_value : 0;

            uint64_t prediction;
            bool use_fcm;

            use_fcm = (i == 0) || (__builtin_clzll(true_value ^ fcm_prediction) > __builtin_clzll(true_value ^ dfcm_difference_prediction));
            prediction = use_fcm ? fcm_prediction : dfcm_difference_prediction;

            uint64_t xor_result = true_value ^ prediction;

            int leading_zero_bytes = __builtin_clzll(xor_result) / 8;
            leading_zero_bytes = (leading_zero_bytes == 4) ? 3 : leading_zero_bytes;

            uint8_t code = (leading_zero_bytes & 0b111) | (use_fcm ? 0b1000 : 0);

            if (i % 2 == 0)
                compressed.push_back(code << 4);
            else
                compressed.back() |= code;

            for (int j = 0; j < 8 - leading_zero_bytes; ++j)
            {
                compressed.push_back((xor_result >> (8 * (7 - j))) & 0xFF);
            }

            state.fcm[state.fcm_hash] = true_value;
            state.fcm_hash = ((state.fcm_hash << 6) ^ (true_value >> 48)) & (TABLE_SIZE - 1);

            if (i > 0)
            {
                uint64_t difference = true_value - state.last_value;
                state.dfcm[state.dfcm_hash] = difference;
                state.dfcm_hash = ((state.dfcm_hash << 2) ^ (difference >> 40)) & (TABLE_SIZE - 1);
            }

            state.last_value = true_value;
        }

        if (input.size() % 2 != 0)
        {
            compressed.push_back(0x00);
        }

        auto encoded = encode(compressed);
        return encoded;
    }

    std::vector<double> compressorDecompressor::decompress(std::vector<uint8_t> &compressed, size_t originalSize)
    {

        auto fpcCpmpreesed = decode(compressed, compressed.size());

        predictorState state;
        state.reset();

        std::vector<double> decompressed;
        size_t byte_index = 0;

        for (size_t i = 0; i < originalSize; ++i)
        {
            uint8_t code = (i % 2 == 0)
                               ? (fpcCpmpreesed[byte_index] >> 4)
                               : (fpcCpmpreesed[byte_index] & 0x0F);

            int leading_zero_bytes = code & 0b111;
            bool switch_predictor = (code & 0b1000);

            uint64_t remainder = 0;
            int remainder_bytes = 8 - leading_zero_bytes;
            for (int j = 0; j < remainder_bytes; ++j)
            {
                remainder = (remainder << 8) | fpcCpmpreesed[byte_index + 1 + j];
            }

            uint64_t prediction = switch_predictor ? state.fcm[state.fcm_hash] : state.dfcm[state.dfcm_hash] + state.last_value;

            uint64_t true_value = prediction ^ remainder;
            decompressed.push_back(*reinterpret_cast<double *>(&true_value));

            byte_index += 1 + remainder_bytes;

            state.fcm[state.fcm_hash] = true_value;
            state.fcm_hash = ((state.fcm_hash << 6) ^ (true_value >> 48)) & (TABLE_SIZE - 1);

            if (i > 0)
            {
                uint64_t difference = true_value - state.last_value;
                state.dfcm[state.dfcm_hash] = difference;
                state.dfcm_hash = ((state.dfcm_hash << 2) ^ (difference >> 40)) & (TABLE_SIZE - 1);
            }

            state.last_value = true_value;
        }

        return decompressed;
    }

}
