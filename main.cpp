#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

#include "CountingScalableBloomFilter.h"

using namespace BloomFilterModels;
using namespace std;

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

class Tester {
    private:
        CountingScalableBloomFilter cbf;
        // chrono::duration<double> elapsed;

        vector<uint8_t> getAsciiBytes(const string& str) {
            vector<uint8_t> bytes(str.begin(), str.end());
            return bytes;
        }
    public:
        Tester(CountingScalableBloomFilter cbf) {
            this->cbf = cbf;
        }
        //TODO measure csbf's size
        //todo gives out optimal m and k
        //todo try other set of m,k,s,fpRate
        //todo design a generic output and logs
        //TODO generate 1M data (unique by line)
        //todo make Random set, Disjoint Set and Same set 

        //todo => make the table

        // Return capacity of csbf
        uint32_t Capacity() {
            return cbf.Capacity();
        }

        // Return number of hash functions
        uint32_t HashFunctionCount() {
            return cbf.K();
        }

        // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
        chrono::duration<double> testAdding(vector<string> dataArray) {
            chrono::duration<double> total_elapsed;
            cout << "Testing Add function of bloom" << endl;
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                cbf.Add(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            }
            return total_elapsed;
        }

        // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
        chrono::duration<double> testCheck(vector<string> dataArray) {
            chrono::duration<double> total_elapsed;
            cout << "Testing Test function of bloom" << endl;
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                cbf.Test(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            }
            return total_elapsed;
        }

        // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
        chrono::duration<double> testRemove(vector<string> dataArray) {
            chrono::duration<double> total_elapsed;
            cout << "Testing Remove function of bloom" << endl;
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                cbf.TestAndRemove(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            }
            return total_elapsed;
        }

        
        ~Tester() {
        }
};

int main()
{
    CountingScalableBloomFilter cbf;

    vector<string> csvData = readCSV("MOCK_DATA.csv");
    

    return 0;
}