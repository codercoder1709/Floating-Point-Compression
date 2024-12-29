#ifndef DATA_PROCESSING_HPP
#define DATA_PROCESSING_HPP

#include <vector>
#include <map>
#include <unordered_map>
#include <numeric>
#include <cstdint>
#include <memory>
using namespace std;
namespace compression
{
    class compressorDecompressor
    {
    public:
        std::vector<uint8_t> compress(const std::vector<double> &input);
        std::vector<double> decompress(std::vector<uint8_t> &compressed, size_t originalSize);

    private:
        const static uint32_t TABLE_SIZE = 1 << 16;
        static constexpr uint32_t RANS_PRECISION = 16;
        static constexpr uint32_t RANS_NORMALIZATION_BITS = 31;
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

        struct SymbolStats
        {
            uint32_t frequencyArray[256];
            uint32_t commulativeFrequency[257];

            void calculateFrequency(vector<uint8_t> &inputArray);
            void calculateCummulativeFrequency();
            void normaliseFrequency(uint32_t totalTarget);
        };

        struct encoderSymbol
        {
            uint32_t upperBound;
            uint32_t frequencyInverse;
            uint32_t bias;
            uint16_t frequencyCompliment;
            uint16_t reciprocalShift;
        };

        struct decoderSymbol
        {
            uint16_t start;
            uint16_t frequency;
        };

        std::vector<uint8_t> encode(const std::vector<uint8_t> &data);

        std::vector<uint8_t> decode(std::vector<uint8_t> &data, size_t original_size);
    };
}

#endif // DATA_PROCESSING_HPP

// uint8_t decode_symbol(uint64_t &x);

// std::unordered_map<uint8_t, uint32_t> freq_table;
// std::unordered_map<uint8_t, uint32_t> cumulative_freq;
// uint32_t total_freq = 0;
// uint64_t state = 0;

// void countFrequency(const std::vector<uint8_t> &data,uint32_t totalTarget);
// static constexpr size_t logOfTableSize = 16;
//         class FcmPredictor
//         {
//             public:
//                 FcmPredictor(int logOfTableSize);
//                 uint64_t getPrediction() const;
//                 void update(uint64_t true_value);

//             private:
//                 std::vector<uint64_t> table;
//                 int fcm_hash;
//         };

//         class DfcmPredictor
//         {
//             public:
//                 DfcmPredictor(int logOfTableSize);
//                 uint64_t getPrediction() const;
//                 void update(uint64_t true_value);

//             private:
//                 std::vector<uint64_t> table;
//                 int dfcm_hash;
//                 uint64_t lastValue;
//         };

//         FcmPredictor predictor1;
//         DfcmPredictor predictor2;