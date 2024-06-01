#include "CountingScalableBloomFilter.h"

using namespace BloomFilterModels;
using namespace std;

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

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
}

void printVector(const vector<string>& data)
{
    for (const auto& str : data)
    {
        cout << str << endl;
    }
}

int main()
{
    CountingScalableBloomFilter cbf;

    vector<string> csvData = readCSV("MOCK_DATA.csv");
    printVector(csvData);
    

    return 0;
}