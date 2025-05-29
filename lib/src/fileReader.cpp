#include "fileReader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


CSVReader::CSVReader(const std::string &file) : filename(file)
{
    readFile();
}

void CSVReader::readFile()
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    data.clear();

    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
        {
            row.push_back(cell);
        }
        data.push_back(row);
    }
}

std::vector<std::string> CSVReader::extractColumn(size_t columnIndex)
{
    std::vector<std::string> column;
    for (const auto &row : data)
    {
        if (columnIndex < row.size())
        {
            column.push_back(row[columnIndex]);
        }
    }
    return column;
}

std::vector<std::string> CSVReader::extractColumnByName(const std::string &columnName)
{
    if (data.empty())
        return {};

    auto header = data[0];
    auto it = std::find(header.begin(), header.end(), columnName);
    if (it != header.end())
    {
        size_t columnIndex = std::distance(header.begin(), it);
        return extractColumn(columnIndex);
    }
    return {};
}

std::vector<float> convertToFloat(const std::vector<std::string> &stringColumn)
{
    std::vector<float> floatColumn;
    for (const auto &str : stringColumn)
    {
        try
        {
            float value = std::stof(str);
            floatColumn.push_back(value);
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Could not convert: " << str << std::endl;
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Out of range: " << str << std::endl;
        }
    }
    return floatColumn;
}
