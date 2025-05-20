#pragma once
#include "Structures.hpp"
#include "fileProcess.hpp"
#include "logger.h"



void logResult(vector<shared_ptr<TestCase>> tcs)
{
    for (auto tc : tcs)
    {
        MonitorLogger::log({{"id", to_string(tc->id)},
                            {"filter", tc->filterName},
                            {"algo", tc->algo},
                            {"scheme", tc->scheme},
                            {"avg_insert_time", tc->avg_insert_time},
                            {"avg_test_time", tc->avg_test_time},
                            {"fp_log", tc->accuracy_log},
                            {"memory", to_string(tc->bf_memory)}});
    }

    MonitorLogger::log({{"id", to_string(std::time(nullptr))},
                        {"filter", "Binary_Search"},
                        {"avg_insert_time", tcs.back()->avg_insert_time},
                        {"avg_test_time", tcs.back()->avg_test_time},
                        {"fp_log", "all_1"},
                        {"memory", to_string(tcs.back()->bs_memory)}});
};

void saveBitmap(const BloomFilterModels::AbstractFilter* filter)
{
    cout << "Saving bitmap to file!" << endl;
    saveBitmapToCSV(filter);
    cout << "Bitmap saved!" << endl;
};

template <typename... Vectors>
std::vector<std::string> mergeVectors(const Vectors&... vectors) {  
    std::vector<std::string> mergedVector;

    // Helper lambda function to insert all elements from a vector into mergedVector
    auto insertAll = [&mergedVector](const auto& vector) {
        mergedVector.insert(mergedVector.end(), vector.begin(), vector.end());
    };

    (insertAll(vectors), ...);

    return mergedVector;
};

template <typename... Vectors>
std::vector<shared_ptr<TestCase>> mergeTcs(const Vectors&... vectors) {  
    std::vector<shared_ptr<TestCase>> mergedVector;

    // Helper lambda function to insert all elements from a vector into mergedVector
    auto insertAll = [&mergedVector](const auto& vector) {
        mergedVector.insert(mergedVector.end(), vector.begin(), vector.end());
    };

    (insertAll(vectors), ...);

    return mergedVector;
};

std::vector<std::string> getShuffledFullsetCopy(const std::vector<std::string>& fullset) {
    std::vector<std::string> copy = fullset; // clone original
    std::random_device rd;                   // obtain random seed
    std::mt19937 g(rd());                    // initialize Mersenne Twister
    std::shuffle(copy.begin(), copy.end(), g); // shuffle in-place
    return copy;
};

void printVector(vector<string> data) {
    for (auto d : data) {
        cout << d << endl;
    }
};

vector<vector<string>> partitionKeys(const vector<string>& keys, size_t X) {
    vector<vector<string>> result;
    for (size_t i = 0; i < keys.size(); i += X) {
        vector<string> chunk;
        for (size_t j = i; j < i + X && j < keys.size(); ++j) {
            chunk.push_back(keys[j]);
        }
        result.push_back(chunk);
    }
    return result;
};

vector<string> readCSV(const string& filename)
{
    vector<string> data;
    ifstream file(filename);
    string line;
    
    while (getline(file, line))
    {
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ','))
        {
            data.push_back(cell);
        }
    }
    
    return data;
};

// Helper function: Extracts the ID (first part of the string)
string extractID(const string& s) {
    return s.substr(0, s.find(' '));
};

vector<uint8_t> getAsciiBytes(const string& str) {
    vector<uint8_t> bytes(str.begin(), str.end());
    // cout << "String: " << str << endl;
    // cout << "Bytes:" << bytes.size() << endl;
    // for (uint8_t byte : bytes) 
        //     cout << static_cast<int>(byte) << ' ';
    // }
    // cout << endl;
    return bytes;
};