#pragma once
#ifndef CSV_READER_HPP
#define CSV_READER_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

class CSVReader
{
private:
    std::string filename;
    std::vector<std::vector<std::string>> data;

    void readFile();

public:
    explicit CSVReader(const std::string &file);

    std::vector<std::string> extractColumn(size_t columnIndex);
    std::vector<std::string> extractColumnByName(const std::string &columnName);
};

// Function to convert a vector of strings to a vector of floats
std::vector<float>convertToFloat(const std::vector<std::string> &stringColumn);

#endif 
