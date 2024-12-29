#include "dataProcessing.hpp"
#include <iostream>
#include <random>
using namespace std;
int main(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 10.0);

    std::vector<double> originalData(1000);
    for (auto& num : originalData) num = dis(gen);
    compression::compressorDecompressor compressor_decompressor;
    auto compressed = compressor_decompressor.compress(originalData);

    auto decompressed = compressor_decompressor.decompress(compressed, originalData.size());
   
    bool isCorrect = true;
    for (size_t i = 0; i < originalData.size(); ++i) {
        if (std::abs(originalData[i] - decompressed[i]) > 1e-10) {
            std::cout << "Mismatch at index " << i 
                      << ": Original = " << originalData[i] 
                      << ", Decompressed = " << decompressed[i] << std::endl;
            isCorrect = false;
        }
    }

    std::cout << (isCorrect ? "Compression/Decompression successful!" : "Compression failed!") << std::endl;
    std::cout << "Original size: " << originalData.size() << " doubles\n";
    std::cout << "Compressed size: " << compressed.size() << " bytes\n";
    std::cout << "Compression ratio: " << (double)originalData.size() * 8 / compressed.size() << "\n\n";

    
    return 0;
}