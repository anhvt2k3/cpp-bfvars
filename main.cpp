#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>

#include "csbf/index.h"
#include "utils/fileProcess.h"
#include <unordered_set>
using namespace std;
using namespace BloomFilterModels;

string set0 = "./data/dataset0.csv"; //# 0 => all possible values
string set1_namefile = "./data/dataset1.csv"; //# 1 => values: 0 -> 200k
string set2_namefile = "./data/dataset2.csv"; //# 2 => values: 200k -> 400k
string set3_namefile = "./data/dataset3.csv"; //# 3 => values: 400k -> 600k
string set4_namefile = "./data/dataset4.csv"; //# 4 => values: 600k -> 800k
string set5_namefile = "./data/dataset5.csv"; //# 5 => values: 800k -> 1M


string algo = Defaults::HASH_ALGORITHM;
string scheme = Defaults::HASH_SCHEME;

string getVarName(string str) {
    if (str == "./data/dataset0.csv") return "set0";
    if (str == "./data/dataset1.csv") return "set1";
    if (str == "./data/dataset2.csv") return "set2";
    if (str == "./data/dataset3.csv") return "set3";
    if (str == "./data/dataset4.csv") return "set4";
    if (str == "./data/dataset5.csv") return "set5";
    return "Unknown";
}

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

// Helper function: Extracts the ID (first part of the string)
string extractID(const string& s) {
    return s.substr(0, s.find(' '));
}

vector<string> set1 = readCSV(set1_namefile); //# 1 => values: 0 -> 200k
vector<string> set2 = readCSV(set2_namefile); //# 2 => values: 200k -> 400k
vector<string> set3 = readCSV(set3_namefile); //# 3 => values: 400k -> 600k
vector<string> set4 = readCSV(set4_namefile); //# 4 => values: 600k -> 800k
vector<string> set5 = readCSV(set5_namefile); //# 5 => values: 800k -> 1M

class Result 
{
    public:
    long long int testCount = 0;
        long long int nof_collision = 0;
        long long int nof_removable = 0;
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
    vector<string> keys;

    vector<string> nonkeys;

    vector<string> fullset = mergeVectors((set1),(set2),(set3),(set4),(set5));
    double binsearch_operatetime;

    vector<uint8_t> getAsciiBytes(const string& str) {
        vector<uint8_t> bytes(str.begin(), str.end());
        // cout << "String: " << str << endl;
        // cout << "Bytes:" << bytes.size() << endl;
        // for (uint8_t byte : bytes) 
            //     cout << static_cast<int>(byte) << ' ';
        // }
        // cout << endl;
        return bytes;
    }

    Tester(BloomFilterModels::AbstractFilter& bf) : bf(bf) {
        cout << "Tester object created!" << endl;
    }

    // keys: set1, set2, set3, set4 || nonkeys: set5
    void initTester800() {
        auto data = mergeVectors((set1), (set2), (set3), (set4));
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        this->nonkeys = (set5);
        this->getEntrySize();
    }

    // keys: set1 || nonkeys: set2, set3, set4
    void initTester200() {
        auto data = mergeVectors((set1));
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        this->nonkeys = mergeVectors((set2), (set3), (set4));
        this->getEntrySize();
    }

    // keys: set1, set2 || nonkeys: set3, set4
    void initTester400() {
        auto data = mergeVectors((set1), (set2));
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        this->nonkeys = mergeVectors((set3), (set4));
        this->getEntrySize();
    }

    // keys: set1, set2 || nonkeys: set3, set4
    void initTester600() {
        auto data = mergeVectors((set1), (set2), (set3) );
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        this->nonkeys = mergeVectors( (set4), (set5) );
        this->getEntrySize();
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

// Function: Insert and Sort
void BinarySearchWrite(vector<string> dataarr) {
    auto start = chrono::high_resolution_clock::now();

    for (const auto& item : dataarr) {
        keys.push_back(item);
    }

    // Sorting based on extracted ID
    sort(keys.begin(), keys.end(), [](const string& a, const string& b) {
        return extractID(a) < extractID(b);
    });

    auto end = chrono::high_resolution_clock::now();
    binsearch_operatetime = chrono::duration<double>(end - start).count();

    cout << "[INFO] BinarySearchWrite: Inserted " << dataarr.size() << " elements. Time: " << binsearch_operatetime << "s\n";
}

// Function: Search and Log
chrono::duration<double> BinarySearchReadTime(vector<string> dataarr) {
    auto start = chrono::high_resolution_clock::now();
    
    int founds = 0;
    for (const auto& item : dataarr) {
        bool found = binary_search(keys.begin(), keys.end(), item);
        // bool found = binary_search(keys.begin(), keys.end(), item, [](const string& a, const string& b) {
        //     return extractID(a) < extractID(b);
        // });
        founds += found;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "[INFO] BinarySearchReadTime: Found = "<<founds<<"\n";

    return duration;
}

// Function: Remove and Sort
void BinarySearchRemove(const vector<string>& subtractArray) {
    auto start = chrono::high_resolution_clock::now();
    
    unordered_set<string> toRemove;  
    for (const auto& item : subtractArray) {
        toRemove.insert(extractID(item));  // Store IDs only
    }

    auto it = remove_if(keys.begin(), keys.end(), [&toRemove](const string& key) {
        return toRemove.count(extractID(key)) > 0;
    });

    keys.erase(it, keys.end());

    // Sorting after deletion
    sort(keys.begin(), keys.end(), [](const string& a, const string& b) {
        return extractID(a) < extractID(b);
    });

    auto end = chrono::high_resolution_clock::now();
    binsearch_operatetime = chrono::duration<double>(end - start).count();

    cout << "[INFO] BinarySearchRemove: Removed " << subtractArray.size() << " elements. Time: " << binsearch_operatetime << "s\n";
}
    /*
        Input: vector of strings || Output: chrono::duration<double> as total time elapsed
        Reset the filter then insert
    */
    chrono::duration<double> testAdding(vector<string> dataArray) {
        chrono::duration<double> total_elapsed(0);
        cout << "Testing Adding of "<< dataArray.size() <<" keys!" << endl;

        if (bf.getFilterName() == "XorFilter") {
            vector<vector<uint8_t>> data;
            for (auto d : dataArray) {
                data.push_back(getAsciiBytes(d));
            }
            auto start = chrono::high_resolution_clock::now();
            bf.Init(data);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        } else {
            if (BloomFilterModels::StaticFilter* sf = dynamic_cast<BloomFilterModels::StaticFilter*>(&bf)) {
                auto start = chrono::high_resolution_clock::now();
                sf->Init(
                    dataArray.size(), Defaults::BUCKET_SIZE, Defaults::FALSE_POSITIVE_RATE, 
                    0, 0, algo, scheme
                );
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                bf.Add(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
                // cout << "Adding: " << dataBytes.data() << endl;
            }
            
            } else {
// * Dynamic Filter will need this to erradicate 1 key exist accoss multiple filter (no matter if it is just an FP or not)
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                bf.TestAndAdd(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            }
        }
        return total_elapsed;
        }
    }

    Result testAdding_v1(vector<string> dataArray) {
        Result res;
        res.nof_collision = 0;

        chrono::duration<double> total_elapsed(0);
        cout << "Testing Adding of "<< dataArray.size() <<" keys!" << endl;

        if (bf.getFilterName() == "XorFilter") {
            vector<vector<uint8_t>> data;
            for (auto d : dataArray) {
                data.push_back(getAsciiBytes(d));
            }
            auto start = chrono::high_resolution_clock::now();
            bf.Init(data);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        }
            if (BloomFilterModels::StaticFilter* sf = dynamic_cast<BloomFilterModels::StaticFilter*>(&bf)) {
                auto start = chrono::high_resolution_clock::now();
                sf->Init(
                    dataArray.size(), Defaults::BUCKET_SIZE, Defaults::FALSE_POSITIVE_RATE, 
                    0, 0, algo, scheme
                );
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                res.nof_collision += bf.Test(dataBytes) ;
                auto start = chrono::high_resolution_clock::now();
                bf.Add(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
                // cout << "Adding: " << dataBytes.data() << endl;
            }
            
            } else {
        // * Dynamic Filter will need this to erradicate 1 key exist accoss multiple filter (no matter if it is just an FP or not)
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                res.nof_collision += bf.Test(dataBytes) ;
                auto start = chrono::high_resolution_clock::now();
                bf.TestAndAdd(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            }
        }
        
        res.elapsed = total_elapsed;
        res.testCount = dataArray.size();
        return res;
    }

    chrono::duration<double> testInserting(vector<string> dataArray) {
        chrono::duration<double> total_elapsed(0);
        cout << "Testing Inserting of "<< dataArray.size() <<" keys!" << endl;

        for (auto data : dataArray) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            auto start = chrono::high_resolution_clock::now();
            bf.Add(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
            // cout << "Adding: " << dataBytes.data() << endl;
        }
        return total_elapsed;
    }

    Result testInserting_v1(vector<string> dataArray) {
        Result res;
        res.nof_collision = 0;
        chrono::duration<double> total_elapsed(0);
        cout << "Testing Inserting of "<< dataArray.size() <<" keys!" << endl;

        for (auto data : dataArray) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            res.nof_collision += bf.Test(dataBytes);
            auto start = chrono::high_resolution_clock::now();
            bf.Add(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
            // cout << "Adding: " << dataBytes.data() << endl;
        }

        res.testCount = dataArray.size();
        res.elapsed = total_elapsed;
        return res;
    }

    // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
    chrono::duration<double> testCheck(vector<string> dataArray) {
        chrono::duration<double> total_elapsed(0);
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
        chrono::duration<double> total_elapsed(0);
        // cout << endl;
        cout << "Testing Removing of "<< dataArray.size()<<" entries!" << endl;
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

    Result testRemove_v1(vector<string> dataArray) {
        Result res;
        res.nof_removable = 0;
        
        auto total_elapsed = chrono::duration<double> (0) ;
        // cout << endl;
        cout << "Testing Removing of "<< dataArray.size()<<" entries!" << endl;
        // cout << endl;
        for (auto data : dataArray) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            auto start = chrono::high_resolution_clock::now();
            res.nof_removable += bf.TestAndRemove(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        }
        
        res.elapsed = total_elapsed;
        res.testCount = dataArray.size();
        return res;
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
        this->initTester200();

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
        result = TestFP((set1), true);
        fcount += result.FP.size();
        cout << "FN count for set1: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set2), false);
        fcount += result.FP.size();
        cout << "FP count for set2: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set3), false);
        fcount += result.FP.size();
        cout << "FP count for set3: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set4), false);
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
        this->initTester400();

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
        result = TestFP((set1), true);
        fcount += result.FP.size();
        cout << "FN count for set1: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set2), true);
        fcount += result.FP.size();
        cout << "FN count for set2: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set3), false);
        fcount += result.FP.size();
        cout << "FP count for set3: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set4), false);
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
        this->initTester800();
        
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

        result = TestFP((set1), true);
        fcount += result.FP.size();
        cout << "FN count for set1: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set5), false);
        fcount += result.FP.size();
        cout << "FP count for set5: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set2), true);
        fcount += result.FP.size();
        cout << "FN count for set2: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set3), true);
        fcount += result.FP.size();
        cout << "FN count for set3: " << result.FP.size() << endl;
        testCount += result.testCount;
        time += result.elapsed;

        result = TestFP((set4), true);
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

    void ICISN_testsuite(string _algo, string _scheme) 
    {
        algo = _algo;
        scheme = _scheme;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        // # Initialization
        string perfFilename = "./icisn-csv/icisn-result-" + to_string(millisec) + ".csv";
        string confFilename = "./icisn-csv/icisn-config-" + to_string(millisec) + ".csv";
        ofstream perf(perfFilename);
        ofstream conf(confFilename);
        if (!perf || !conf) {
            cerr << "Failed to open output files" << endl;
            return;
        }

        // Write headers to CSV files
        TestCase tc;
        perf << tc.getHeader();

        this->initTester800();
        cout << endl;

        // # Test node 1
        // Insert set S -> time
        cout << "BEFORE ADDING" << endl;
        auto res = testAdding_v1(keys);
        cout << "AFTER ADDING" << endl;
        auto elapsed = res.elapsed.count();
        cout << "Adding 1 Set Elapsed time: " << elapsed << "s" << endl;
        tc.adding_time = elapsed;
        tc.nof_collision = res.nof_collision;
        tc.nof_operand = res.testCount;

        // # ReadTimeMENT
        long long int fcount = 0;
        long long int testCount = 0;
        Result result_1;
        auto time = chrono::duration<double>(0);

        result_1 = TestFP((set1), true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set2), true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set3), true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set4), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set5), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        float accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);
        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        auto bisearch_time = BinarySearchReadTime(fullset).count();
        cout << "Binary Search Operate Time: " << binsearch_operatetime << "s" << endl;
        cout << "Binary Search Elapsed Time: " << bisearch_time << "s" << endl;
        cout << endl;

        // Record the removal performance into TestCase
        tc.test_case = "InsertSet1,2,3";
        // tc.adding_time = elapsed;
        tc.key_set = "1,2,3";
        tc.nonkey_set = "4,5";
        tc.test_size = testCount;
        tc.accuracy = accuracy;
        tc.test_time = time.count();
        tc.operation_time = binsearch_operatetime;
        tc.binsearch_time = bisearch_time;
        tc.filterID = bf.getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString(); tc.reset();
        // # END ReadTimeMENT

        // # Test node 2
        // Remove set R -> time
        res = testRemove_v1((set1)); 
        BinarySearchRemove((set1));
        elapsed = res.elapsed.count(); 
        cout << "Removing 1 Set Elapsed time: " << elapsed << "s" << endl;
        tc.adding_time = elapsed;
        tc.nof_removable = res.nof_removable;
        tc.nof_operand = res.testCount;

        // # ReadTimeMENT
        // Check set S-R over U -> accuracy
        fcount = 0;
        testCount = 0;
        time = chrono::duration<double>(0);

        result_1 = TestFP((set1), false);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set2), true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set3), true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set4), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set5), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);
        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        bisearch_time = BinarySearchReadTime(fullset).count();
        cout << "Binary Search Operate Time: " << binsearch_operatetime << "s" << endl;
        cout << "Binary Search Elapsed Time: " << bisearch_time << "s" << endl;
        cout << endl;

        // Record the removal performance into TestCase
        tc.test_case = "RemoveSet1";
        tc.key_set = "2,3";
        tc.nonkey_set = "1,4,5";
        tc.test_size = testCount;
        tc.accuracy = accuracy;
        tc.test_time = time.count();
        tc.operation_time = binsearch_operatetime;
        tc.binsearch_time = bisearch_time;
        tc.filterID = bf.getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString(); tc.reset();
        // # END ReadTimeMENT

        // # Test node 3
        // Insert set S -> time
        res = testInserting_v1((set1));
        BinarySearchWrite((set1));
        elapsed = res.elapsed.count();
        cout << "Inserting 1 Set Elapsed time: " << elapsed << "s" << endl;
        tc.adding_time = elapsed;
        tc.nof_collision = res.nof_collision;
        tc.nof_operand = res.testCount;

        // # ReadTimeMENT
        fcount = 0;
        testCount = 0;
        time = chrono::duration<double>(0);

        result_1 = TestFP((set1), true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set2), true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set3), true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set4), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set5), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);
        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        bisearch_time = BinarySearchReadTime(fullset).count();
        cout << "Binary Search Operate Time: " << binsearch_operatetime << "s" << endl;
        cout << "Binary Search Elapsed Time: " << bisearch_time << "s" << endl;
        cout << endl;

        // Record the removal performance into TestCase
        tc.test_case = "InsertSet1";
        // tc.adding_time = elapsed;
        tc.key_set = "1,2,3";
        tc.nonkey_set = "4,5";
        tc.test_size = testCount;
        tc.accuracy = accuracy;
        tc.test_time = time.count();
        tc.operation_time = binsearch_operatetime;
        tc.binsearch_time = bisearch_time;
        tc.filterID = bf.getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString(); tc.reset();
        // # END ReadTimeMENT
        
        // # Test node 4
        // Insert set S -> time
        res = testInserting_v1((set4));
        BinarySearchWrite((set4));
        elapsed = res.elapsed.count();
        cout << "Inserting Set 4 Elapsed time: " << elapsed << "s" << endl;
        tc.adding_time = elapsed;
        tc.nof_collision = res.nof_collision;
        tc.nof_operand = res.testCount;

        // # ReadTimeMENT
        fcount = 0;
        testCount = 0;
        time = chrono::duration<double>(0);

        result_1 = TestFP((set1), true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set2), true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set3), true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set4), true);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((set5), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);
        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        bisearch_time = BinarySearchReadTime(fullset).count();
        cout << "Binary Search Operate Time: " << binsearch_operatetime << "s" << endl;
        cout << "Binary Search Elapsed Time: " << bisearch_time << "s" << endl;
        cout << endl;

        // Record the removal performance into TestCase
        tc.test_case = "InsertSet4";
        // tc.adding_time = elapsed;
        tc.key_set = "1,2,3,4";
        tc.nonkey_set = "5";
        tc.test_size = testCount;
        tc.accuracy = accuracy;
        tc.test_time = time.count();
        tc.operation_time = binsearch_operatetime;
        tc.binsearch_time = bisearch_time;
        tc.filterID = bf.getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString(); tc.reset();
        // # END ReadTimeMENT

        Configuration cf(&bf); conf << cf.getHeader();
        cf.bs_memory = sizeof(keys);
        conf << cf.toCSVString();

        // # Finalization

        cout << "Test done running!" << endl;

        perf.close();
        conf.close();
    }

    string AAdd600k (
                vector<string> skey1 = set1, 
                vector<string> skey2 = set2, 
                vector<string> skey3 = set3, 
                vector<string> snkey1 = set4,
                vector<string> snkey2 = set5
        )
    {
        TestCase tc;
        // # Test node 1
        // Insert set S -> time
        auto elapsed = testAdding(mergeVectors((skey1),(skey2),(skey3))).count();
        cout << "Adding Keys Elapsed time: " << elapsed << "s" << endl;
        tc.filterID = bf.getFilterCode();
        tc.adding_time = elapsed;

        // # ReadTimeMENT
        long long int fcount = 0;
        long long int testCount = 0;
        Result result_1;
        auto time = chrono::duration<double>(0);

        result_1 = TestFP((skey1), true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((skey2), true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((skey3), true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((snkey1), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((snkey2), false);
        fcount += result_1.FP.size();
        tc.f5 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        float accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);
        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        cout << endl;

        // Record the removal performance into TestCase
        tc.test_case = "Std Test";
        // tc.adding_time = elapsed;
        // # to be fixed
        tc.key_set = getVarName("skey1") + "+" + getVarName("skey2") + "+" + getVarName("skey3");
        tc.nonkey_set = getVarName("snkey1") + "+" + getVarName("snkey2");
        tc.test_size = testCount;
        tc.accuracy = accuracy;
        tc.test_time = time.count();
        // # END ReadTimeMENT

        cout << getConfig();
        cout << "Test done running!" << endl;
        cout << endl;
        return tc.toCSVString();
    }
 
    string AAdd400k (
                vector<string> skey1 = set1, 
                vector<string> skey2 = set2, 
                vector<string> snkey1 = set3, 
                vector<string> snkey2 = set4,
                vector<string> snkey3 = set5
        )
    {
        TestCase tc;
        // # Test node 1
        // Insert set S -> time
        auto elapsed = testAdding(mergeVectors((skey1),(skey2))).count();
        cout << "Adding Keys Elapsed time: " << elapsed << "s" << endl;
        tc.filterID = bf.getFilterCode();
        tc.adding_time = elapsed;

        // # ReadTimeMENT
        long long int fcount = 0;
        long long int testCount = 0;
        Result result_1;
        auto time = chrono::duration<double>(0);

        result_1 = TestFP((skey1), true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((skey2), true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((snkey1), false);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((snkey2), false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP((snkey3), false);
        fcount += result_1.FP.size();
        tc.f5 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        float accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
        cout << fixed << setprecision(6);
        cout << "False Count: " << fcount << " -- Accuracy: " << accuracy << endl;
        cout << "Total Test Count: " << testCount << endl;
        cout << "Total Elapsed Time: " << time.count() << "s" << endl;
        cout << endl;

        // Record the removal performance into TestCase
        tc.test_case = "Std Test 2";
        // tc.adding_time = elapsed;
        // # to be fixed
        tc.key_set = getVarName("skey1") + "+" + getVarName("skey2") ;
        tc.nonkey_set = getVarName("snkey1") + "+" + getVarName("snkey2") + "+" + getVarName("snkey3");
        tc.test_size = testCount;
        tc.accuracy = accuracy;
        tc.test_time = time.count();
        // # END ReadTimeMENT

        cout << getConfig();
        cout << "Test done running!" << endl;
        cout << endl;
        return tc.toCSVString();
    }
 
    ~Tester() {
    }
};

void saveBitmap(const BloomFilterModels::AbstractFilter* filter)
{
    cout << "Saving bitmap to file!" << endl;
    saveBitmapToCSV(filter);
    cout << "Bitmap saved!" << endl;
}

// #define AtomicI
// #define AtomicII
#define Icisn
int main()
{
#ifdef AtomicI
    ofstream perf("csvs/stdsuite-result.csv");
    ofstream conf("csvs/stdsuite-config.csv");
    if (!perf || !conf) {
        cerr << "Cannot open file: std800k-result.csv or std800k-config.csv" << endl;
        return 1;
    }
    Configuration cf;
    TestCase tc;

    perf << tc.getHeader();
    conf << cf.getHeader();

    for (auto filter : filters) {
        cout << "Filter: " << filter->getFilterCode() << endl;
        Tester tester(*filter);
        Configuration cf(filter);
        // tester.initTester800();
        // tester.getEntrySize();
        // tester.Testsuite800keys();

        perf << tester.AAdd600k();
        filter->Reset();
        perf << tester.AAdd600k(set1,set3,set4,set2,set5);
        filter->Reset();
        perf << tester.AAdd600k(set2,set4,set3,set5,set1);
        filter->Reset();
        perf << tester.AAdd600k(set3,set5,set2,set4,set1);
        filter->Reset();
        perf << tester.AAdd600k(set3,set2,set5,set4,set1);
        // tester.initTester200();
        // tester.getEntrySize();
        // tester.Testsuite200keys();
        // saveBitmap(filter);
        conf << cf.toCSVString();
        cout << "-------------END-------------" << endl;
        cout << endl;
    }

    conf.close();
    perf.close();
#endif
#ifdef AtomicII
    ofstream perf("csvs/stdsuite-2-result.csv");
    ofstream conf("csvs/stdsuite-2-config.csv");
    if (!perf || !conf) {
        cerr << "Cannot open file: std800k-result.csv or std800k-config.csv" << endl;
        return 1;
    }
    Configuration cf;
    TestCase tc;

    perf << tc.getHeader();
    conf << cf.getHeader();

    for (auto filter : filters) {
        cout << "Filter: " << filter->getFilterCode() << endl;
        Tester tester(*filter);
        Configuration cf(filter);
        // tester.initTester800();
        // tester.getEntrySize();
        // tester.Testsuite800keys();

        perf << tester.AAdd400k();
        filter->Reset();
        perf << tester.AAdd400k(set1,set3,set4,set2,set5);
        filter->Reset();
        perf << tester.AAdd400k(set2,set4,set3,set5,set1);
        filter->Reset();
        perf << tester.AAdd400k(set3,set5,set2,set4,set1);
        filter->Reset();
        perf << tester.AAdd400k(set3,set2,set5,set4,set1);
        // tester.initTester200();
        // tester.getEntrySize();
        // tester.Testsuite200keys();
        // saveBitmap(filter);
        conf << cf.toCSVString();
        cout << "-------------END-------------" << endl;
        cout << endl;
    }

    conf.close();
    perf.close();
#endif
#ifdef Icisn
    // * Testmode : default
    string hashFuncs[] = {
        Hash32::ALGO_MURMUR3_32,
        Hash32::ALGO_MURMUR3_128,
        Hash32::ALGO_FNV1A,
        Hash32::ALGO_SIPHASH,
    };

    string hashSchemes[] = {
        Hash32::SCHEME_SERIAL,
        Hash32::SCHEME_KIRSCH_MITZENMACHER,
        Hash32::SCHEME_ENHANCED_DOUBLE_HASHING,
    };

    for (auto hashFunc : hashFuncs) {
        for (auto hashScheme : hashSchemes) {
            DeletableBloomFilter *dlbf = new DeletableBloomFilter();
            cout << "Filter: " << dlbf->getFilterCode() << endl;
            Tester tester(*dlbf);
            tester.ICISN_testsuite(hashFunc, hashScheme);
            cout << "Config of filter: " << dlbf->getConfigure() << endl;
            delete(dlbf);
        }
    }

#endif
#ifdef default
    // * Testmode : default
    string hashFuncs[] = {
        Hash32::ALGO_MURMUR3_32,
        Hash32::ALGO_MURMUR3_128,
        // Hash32::ALGO_SHA256,
        // Hash32::ALGO_XXH3,
        Hash32::ALGO_FNV1A,
        Hash32::ALGO_SIPHASH,
    };

    string hashSchemes[] = {
        Hash32::SCHEME_SERIAL,
        Hash32::SCHEME_KIRSCH_MITZENMACHER,
        Hash32::SCHEME_ENHANCED_DOUBLE_HASHING,
    };

    for (auto hashFunc : hashFuncs) {
        for (auto hashScheme : hashSchemes) {
            CountingBloomFilter *scbf = new CountingBloomFilter();
            cout << "Filter: " << scbf->getFilterCode() << endl;
            Tester tester(*scbf);
            tester.ICISN_testsuite(hashFunc, hashScheme);

            delete(scbf);
        }
    }
#endif
    return 0;
}
