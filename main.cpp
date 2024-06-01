#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

#include "csbf/CountingScalableBloomFilter.h"
using namespace BloomFilterModels;
using namespace std;

string setall = "/data/dataset0.csv"; //# 0 => all possible values
string set300 = "/data/dataset1.csv"; //# 1 => values: 0 -> 300k
string set700 = "/data/dataset2.csv"; //# 2 => values: 300k001 -> 1M
string set500 = "/data/dataset3.csv"; //# 3 => values: 0 -> 500k
string set501 = "/data/dataset4.csv"; //# 4 => values: 500k001 -> 1M
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
        //todo try other set of m,k,s,fpRate
        //todo measure FP rate of csbf
        //TODO generate 1M data (unique by line)
        //todo make Random set, Disjoint Set and Same set 

        //todo => make the table

        //todo gives out The csbf instance configures
        string getConfig() {
            return cbf.getConfigure();
        }

        //TODO measure csbf's size
        // Return capacity of csbf
        string Capacity() {
            return to_string(cbf.Capacity());
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
    Tester tester(cbf);
    auto data = readCSV(setall);
    cout << "Elapsed time: " << tester.testAdding(data).count() << "s\n";
    

    return 0;
}