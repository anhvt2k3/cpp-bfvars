#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

#include "csbf/index.h"
using namespace std;
using namespace BloomFilterModels;

string setall = "./data/dataset0.csv"; //# 0 => all possible values
string set1 = "./data/dataset1.csv"; //# 1 => values: 0 -> 200k
string set2 = "./data/dataset2.csv"; //# 2 => values: 200k -> 400k
string set3 = "./data/dataset3.csv"; //# 3 => values: 400k -> 600k
string set4 = "./data/dataset4.csv"; //# 4 => values: 600k -> 800k
string set5 = "./data/dataset5.csv"; //# 5 => values: 800k -> 1M

void printVector(vector<string> data) {
    for (auto d : data) {
        cout << d << endl;
    }
}

template <typename... Vectors>
std::vector<std::string> mergeVectors(const Vectors&... vectors) {
    std::vector<std::string> mergedVector;

    // Helper lambda function to insert all elements from a vector into mergedVector
    auto insertAll = [&mergedVector](const auto& vector) {
        mergedVector.insert(mergedVector.end(), vector.begin(), vector.end());
    };

    (insertAll(vectors), ...);

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

class Result 
{
    public:
        long long int testCount = 0;
        chrono::duration<double> elapsed;
        vector<string> FP;
        float accuracy = 0;
        Result() {}
        void setFP(vector<string> FP) {
            this->FP = FP;
        }
        // This function should be called after all FP is set
        void finalize(long long int size)
        {
            testCount = size;
            this->accuracy = (testCount - FP.size()) / testCount;
        }
        ~Result() {}
};

class Tester {
public:
    BloomFilterModels::AbstractFilter& bf;
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

    Tester(BloomFilterModels::AbstractFilter& bf) : bf(bf) {
        cout << "Tester object created!" << endl;
    }

    void initTester800() {
        this->keys = mergeVectors(readCSV(set1), readCSV(set2), readCSV(set3), readCSV(set4));
        this->nonkeys = readCSV(set5);
    }

    void initTester200() {
        this->keys = mergeVectors(readCSV(set1));
        this->nonkeys = mergeVectors(readCSV(set2), readCSV(set3), readCSV(set4));
    }

    void initTester400() {
        this->keys = mergeVectors(readCSV(set1), readCSV(set2));
        this->nonkeys = mergeVectors(readCSV(set3), readCSV(set4));
    }

    void getEntrySize() {
        cout << "Keys: " << keys.size() << endl;
        cout << "NonKeys: " << nonkeys.size() << endl;
    }

    string getConfig() {
        return bf.getConfigure();
    }

    // Return capacity of bf
    string Capacity() {
        return to_string(bf.Capacity());
    }

    // Return number of hash functions
    uint32_t HashFunctionCount() {
        return bf.K();
    }

    // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
    chrono::duration<double> testAdding(vector<string> dataArray) {
        chrono::duration<double> total_elapsed;
        cout << "Testing Adding of "<< dataArray.size() <<" keys!" << endl;

        if (bf.getFilterName() == "XorFilter") {
            vector<vector<uint8_t>> data;
            for (auto d : dataArray) {
                data.push_back(getAsciiBytes(d));
            }
            auto start = chrono::high_resolution_clock::now();
            bf.Init(data);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed = end - start;
        } else {
            if (BloomFilterModels::StaticFilter* sf = dynamic_cast<BloomFilterModels::StaticFilter*>(&bf)) {
                sf->Init(dataArray.size());
            }
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                bf.Add(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
                // cout << "Adding: " << dataBytes.data() << endl;
            }
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
            bf.Test(dataBytes);
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
            bf.TestAndRemove(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        }
        return total_elapsed;
    }

    Result TestFP(vector<string> dataArray, bool correctAns=true) {
        Result result;
        for (auto data : dataArray) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            auto start = chrono::high_resolution_clock::now();
            if (bf.Test(dataBytes) != correctAns) {
                result.FP.push_back(data);
            }
            auto end = chrono::high_resolution_clock::now();
            result.elapsed += end - start;
        }
        result.finalize(dataArray.size());
        return result;
    }

    void Testsuite200keys()
    {
        cout << endl;
        auto elapsed = testAdding(keys).count();
        cout << "Adding Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        // #      Test = 200k keys       .. 600k not keys
        cout << endl;
        auto adjacent_set1 = mergeVectors(keys, nonkeys);
        elapsed = testCheck(adjacent_set1).count();
        cout << "Check Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        long long int fcount = 0;
        long long int testCount = 0;
        auto time = chrono::duration<double>(0);

        Result result;
        result = TestFP(readCSV(set1), true);
        fcount += result.FP.size();
        cout << "FN count for set1: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set2), false);
        fcount += result.FP.size();
        cout << "FP count for set2: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set3), false);
        fcount += result.FP.size();
        cout << "FP count for set3: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set4), false);
        fcount += result.FP.size();
        cout << "FP count for set4: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        float accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);

        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        cout << endl;

        cout << getConfig() << endl;

        cout << "Test done running!" << endl;
    }

    void Testsuite400keys()
    {
        cout << endl;
        auto elapsed = testAdding(keys).count();
        cout << "Adding Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        // #      Test = 400k keys       .. 600k not keys
        cout << endl;
        auto adjacent_set1 = mergeVectors(keys, nonkeys);
        elapsed = testCheck(adjacent_set1).count();
        cout << "Check Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        // #      Test = 600k not keys   .. 400k keys
        cout << endl;
        auto adjacent_set2 = mergeVectors(nonkeys, keys);
        elapsed = testCheck(adjacent_set2).count();
        cout << "Check Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        long long int fcount = 0;
        long long int testCount = 0;
        auto time = chrono::duration<double>(0);

        Result result;
        result = TestFP(readCSV(set1), true);
        fcount += result.FP.size();
        cout << "FN count for set1: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set2), true);
        fcount += result.FP.size();
        cout << "FN count for set2: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set3), false);
        fcount += result.FP.size();
        cout << "FP count for set3: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set4), false);
        fcount += result.FP.size();
        cout << "FP count for set4: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        float accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);

        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        cout << endl;

        cout << getConfig() << endl;

        cout << "Test done running!" << endl;
    }

    void Testsuite800keys()
    {
        
        cout << endl;
        auto elapsed = testAdding(keys).count();
        cout << "Adding Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        // #      Test = 800k keys       .. 200k not keys
        cout << endl;
        auto adjacent_set1 = mergeVectors(keys, nonkeys);
        elapsed = testCheck(adjacent_set1).count();
        cout << "Check Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        // #      Test = 200k not keys   .. 800k keys
        cout << endl;
        auto adjacent_set2 = mergeVectors(nonkeys, keys);
        elapsed = testCheck(adjacent_set2).count();
        cout << "Check Elapsed time: " << elapsed << "s" << endl;
        cout << endl;

        //? More details with Test = set1 . set5 . set2 . set3 . set4
        cout << endl;
        cout << "   ** 1M entries test for 200k(keys) . 200k(nonkeys) . 600k(keys) **" << endl;
        Result result;

        long long int fcount = 0;
        long long int testCount = 0;
        auto time = chrono::duration<double>(0);

        result = TestFP(readCSV(set1), true);
        fcount += result.FP.size();
        cout << "FN count for set1: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set5), false);
        fcount += result.FP.size();
        cout << "FP count for set5: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set2), true);
        fcount += result.FP.size();
        cout << "FN count for set2: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set3), true);
        fcount += result.FP.size();
        cout << "FN count for set3: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP(readCSV(set4), true);
        fcount += result.FP.size();
        cout << "FN count for set4: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        float accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);

        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        cout << endl;

        cout << getConfig() << endl;

        cout << "Test done running!" << endl;
    }


    ~Tester() {
    }
};

int main()
{
    vector<BloomFilterModels::AbstractFilter*> filters = {};
    // VariableIncrementBloomFilter vibf; filters.push_back(&vibf);
    XorFilter xf; filters.push_back(&xf); //! Stuck in the HashFunction-Data Reconciling process
    // OneHashingBloomFilter ohbf; filters.push_back(&ohbf);
    // DeletableBloomFilter dlbf; filters.push_back(&dlbf);
    // CountingBloomFilter scbf; filters.push_back(&scbf);
    // StandardBloomFilter bf; filters.push_back(&bf);
    // CryptoCountingBloomFilter cbf; filters.push_back(&cbf);
    
    // StandardCountingScalableBloomFilter scsbf; filters.push_back(&scsbf);
    // DynamicStdCountingBloomFilter dsbf(400000); filters.push_back(&dsbf);
    // ScalableDeletableBloomFilter sdlbf; filters.push_back(&sdlbf);
    // CountingScalableBloomFilter csbf; filters.push_back(&csbf);
    // DynamicBloomFilter dbf(400000); filters.push_back(&dbf);
    // vector<BloomFilterModels::AbstractFilter*> filters = {&bf, &cbf, &scbf, &csbf, &scsbf, &dbf, &dsbf};
    for (auto filter : filters) {
        cout << "Filter: " << filter->getFilterCode() << endl;
        Tester tester(*filter);
        // tester.initTester800();
        // tester.getEntrySize();
        // tester.Testsuite800keys();

        tester.initTester400();
        tester.getEntrySize();
        tester.Testsuite400keys();

        // tester.initTester200();
        // tester.getEntrySize();
        // tester.Testsuite200keys();
        cout << "-------------END-------------" << endl;
        cout << endl;
    }
    return 0;
}