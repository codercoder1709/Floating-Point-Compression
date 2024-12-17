#ifndef DATA_PROCESSING_HPP
#define DATA_PROCESSING_HPP

#include <vector>
#include <map>
#include <unordered_map>
#include <numeric>
#include <cstdint>
#include <memory>

namespace compression
{
    class compressorDecompressor
    {
    public:
        std::vector<uint8_t> compress(const std::vector<double> &input);
        std::vector<double> decompress(std::vector<uint8_t> &compressed, size_t originalSize);

    private:
        static constexpr size_t TABLE_SIZE = 1024;
        static constexpr uint32_t RANS_PRECISION = 16;
        static constexpr uint32_t RANS_NORMALIZATION_BITS = 31;

        class predictorState
        {
        public:
            uint64_t fcm[TABLE_SIZE] = {0};
            uint64_t dfcm[TABLE_SIZE] = {0};
            uint64_t fcm_hash = 0;
            uint64_t dfcm_hash = 0;
            uint64_t last_value = 0;

            void reset();
        };

        std::unordered_map<uint8_t, uint32_t> freq_table;
        std::unordered_map<uint8_t, uint32_t> cumulative_freq;
        uint32_t total_freq = 0;
        uint64_t state = 0;

        void countFrequency(const std::vector<uint8_t> &data,uint32_t totalTarget);

        std::vector<uint8_t> encode(const std::vector<uint8_t> &data);

        std::vector<uint8_t> decode(std::vector<uint8_t> &data, size_t original_size);
        
        uint8_t decode_symbol(uint64_t& x);

        
    };
}

#endif // DATA_PROCESSING_HPP