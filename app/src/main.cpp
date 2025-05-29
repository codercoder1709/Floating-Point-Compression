#include "dataProcessing.hpp"
#include "fileReader.hpp"
#include <iostream>
#include <random>

void compressAndVerify(const std::vector<float> &data)
{
    compression::compressorDecompressor compressor_decompressor;
    auto compressed = compressor_decompressor.compress(data);

    auto decompressed = compressor_decompressor.decompress(compressed.first, data.size(), compressed.second);

    bool isCorrect = true;
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (std::abs(data[i] - decompressed[i]) > 1e-10)
        {
            std::cout << "Mismatch at index " << i
                      << ": Original = " << data[i]
                      << ", Decompressed = " << decompressed[i] << std::endl;
            isCorrect = false;
        }
    }

    std::cout << (isCorrect ? "Compression/Decompression successful!" : "Compression failed!") << std::endl;
    std::cout << "Original size: " << data.size() * sizeof(double) << " bytes\n";
    std::cout << "Compressed size: " << compressed.first.size() << " bytes\n";
    std::cout << "Compression ratio: "
              << double(data.size()) * sizeof(double) / compressed.first.size() << "\n\n";
}


int main()
{
    try
    {
        std::string file_path1 = "../../dataset/largeVolume/city_temperature.csv";

        CSVReader csv1(file_path1);
        std::vector<std::string> column1 = csv1.extractColumn(7);

        std::vector<float> originalData1 = convertToFloat(column1);

        std::vector<float> data1(originalData1.begin() + 1, originalData1.begin() + 100001);


        // Call the modular function
        compressAndVerify(data1);
        
        std::string file_path2 = "../../dataset/largeVolume/dataexport_20250126T094958.csv";

        CSVReader csv(file_path2);
        std::vector<std::string> column = csv.extractColumnByName("Basel");
        
        std::vector<float> originalData = convertToFloat(column);

        std::vector<float> data(originalData.begin() + 1, originalData.begin() + 100001);


        // Call the modular function
        compressAndVerify(data);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
