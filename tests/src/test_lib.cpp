#include <gtest/gtest.h>
#include "dataProcessing.hpp"
#include <iostream>
using namespace std;

// // Test that reset() sets all member variables to their initial state
// TEST(PredictorStateResetTest, ResetsAllVariablesToZero) {
//     compression::compressorDecompressor::predictorState predictor;
    
//     // Initialize with some non-zero values before reset
//     predictor.fcm[10] = 42;
//     predictor.dfcm[15] = 99;
//     predictor.fcm_hash = 123;
//     predictor.dfcm_hash = 456;
//     predictor.last_value = 789;

//     // Call reset method
//     predictor.reset();

//     // Check FCM table is zeroed out
//     for (size_t i = 0; i < compression::compressorDecompressor::TABLE_SIZE; ++i) {
//         EXPECT_EQ(predictor.fcm[i], 0) 
//             << "FCM table not reset at index " << i;
//     }

//     // Check DFCM table is zeroed out
//     for (size_t i = 0; i < compression::compressorDecompressor::TABLE_SIZE; ++i) {
//         EXPECT_EQ(predictor.dfcm[i], 0) 
//             << "DFCM table not reset at index " << i;
//     }

//     // Check hash values are reset
//     EXPECT_EQ(predictor.fcm_hash, 0) 
//         << "FCM hash not reset to zero";
//     EXPECT_EQ(predictor.dfcm_hash, 0) 
//         << "DFCM hash not reset to zero";

//     // Check last_value is reset
//     EXPECT_EQ(predictor.last_value, 0) 
//         << "Last value not reset to zero";
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}