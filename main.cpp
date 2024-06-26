#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

#include "csbf/CountingScalableBloomFilter.h"
using namespace BloomFilterModels;
using namespace std;

string setall = "./data/dataset0.csv"; //# 0 => all possible values
string set300 = "./data/dataset1.csv"; //# 1 => values: 0 -> 300k
string set700 = "./data/dataset2.csv"; //# 2 => values: 300k001 -> 1M
string set500 = "./data/dataset3.csv"; //# 3 => values: 0 -> 500k
string set501 = "./data/dataset4.csv"; //# 4 => values: 500k001 -> 1M

void printVector(vector<string> data) {
    for (auto d : data) {
        cout << d << endl;
    }
}

template <typename T>
std::vector<T> mergeVectors(const std::vector<T>& vector1, const std::vector<T>& vector2) {
    std::vector<T> mergedVector;
    mergedVector.reserve(vector1.size() + vector2.size());
    mergedVector.insert(mergedVector.end(), vector1.begin(), vector1.end());
    mergedVector.insert(mergedVector.end(), vector2.begin(), vector2.end());
    return mergedVector;
}

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
    public:
        CountingScalableBloomFilter& csbf;
        // chrono::duration<double> elapsed;
        vector<string> keys;

        vector<string> nonkeys;

        vector<uint8_t> getAsciiBytes(const string& str) {
            vector<uint8_t> bytes(str.begin(), str.end());
            // cout << "String: " << str << endl;
            // cout << "Bytes:" << bytes.size() << endl;
            // for (uint8_t byte : bytes) {
            //     cout << static_cast<int>(byte) << ' ';
            // }
            // cout << endl;
            return bytes;
        }

        Tester(CountingScalableBloomFilter& csbf) : csbf(csbf) {
            // this->csbf = csbf;
            cout << "Tester is created" << endl;
        }

        void setEntry(vector<string> keys, vector<string> nonkeys) {
            this->keys = keys;
            this->nonkeys = nonkeys;
        }

        void getEntrySize() {
            cout << "Keys: " << keys.size() << endl;
            cout << "NonKeys: " << nonkeys.size() << endl;
        }

        //todo measure csbf's size

        //todo try other set of m,k,s,fpRate
        //todo measure FP rate of csbf
        //TODO generate 1M data (unique by line)
        //todo make Random set, Disjoint Set and Same set 

        //todo => make the table

        //todo gives out The csbf instance configures
        string getConfig() {
            return csbf.getConfigure();
        }

        //TODO measure csbf's size
        // Return capacity of csbf
        string Capacity() {
            return to_string(csbf.Capacity());
        }

        // Return number of hash functions
        uint32_t HashFunctionCount() {
            return csbf.K();
        }

        // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
        chrono::duration<double> testAdding(vector<string> dataArray) {
            chrono::duration<double> total_elapsed;
            // cout << endl;
            cout << "Testing Adding of "<< dataArray.size() <<" keys!" << endl;
            // cout << endl;
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                csbf.Add(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
                // cout << "Adding: " << dataBytes.data() << endl;
            }
            return total_elapsed;
        }

        // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
        chrono::duration<double> testCheck(vector<string> dataArray) {
            chrono::duration<double> total_elapsed;
            // cout << endl;
            cout << "Testing Test function for "<< dataArray.size()<<" entries!" << endl;
            // cout << endl;
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                csbf.Test(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            }
            return total_elapsed;
        }

        // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
        chrono::duration<double> testRemove(vector<string> dataArray) {
            chrono::duration<double> total_elapsed;
            // cout << endl;
            cout << "Testing Removing of "<< dataArray.size()<<" removals!" << endl;
            // cout << endl;
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                csbf.TestAndRemove(dataBytes);
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
    auto keys = readCSV(set700);
    auto notkeys = readCSV(set300);
    CountingScalableBloomFilter csbf(keys.size());
    cout << "Debug!" << endl;
    Tester tester(csbf);
    cout << endl;
    auto elapsed = tester.testAdding(keys).count();
    cout << "Adding Elapsed time: " << elapsed << "s" << endl;
    cout << endl;

//#      Test = 700k keys .. 300k not keys
    cout << endl;
    auto adjacent_set1 = mergeVectors(keys, notkeys);
    elapsed = tester.testCheck(adjacent_set1).count();
    cout << "Check Elapsed time: " << elapsed << "s" << endl;
    cout << endl;

//#      Test = 300k not keys .. 700k keys
    cout << endl;
    auto adjacent_set2 = mergeVectors(notkeys, keys);
    elapsed = tester.testCheck(adjacent_set2).count();
    cout << "Check Elapsed time: " << elapsed << "s" << endl;
    cout << endl;

    cout << tester.getConfig() << endl;

    cout << "Everything done running!" <<endl;
    return 0;
}