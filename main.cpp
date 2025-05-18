#include "csbf/index.h"
#include "utils/main_utils.hpp"
#include <unordered_set>

// #define OBSOLETE
// #define AtomicI
// #define AtomicII
// #define Icisn
#define Bus
#define Banking

using namespace std;
using namespace BloomFilterModels;

string set0 = "./data/dataset0.csv"; //# 0 => all possible values
string set1_filename = "./data/dataset1.csv"; //# 1 => values: 0 -> 200k
string set2_filename = "./data/dataset2.csv"; //# 2 => values: 200k -> 400k
string set3_filename = "./data/dataset3.csv"; //# 3 => values: 400k -> 600k
string set4_filename = "./data/dataset4.csv"; //# 4 => values: 600k -> 800k
string set5_filename = "./data/dataset5.csv"; //# 5 => values: 800k -> 1M

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

vector<string> set1;
vector<string> set2;
vector<string> set3;
vector<string> set4;
vector<string> set5;
vector<Set> sets;
vector<string> fullset;

void import_Busticketing_data() {
    set1 = readCSV("data/dataset1.csv"); //# 1 => values: 0 -> 200k
    set2 = readCSV("data/dataset2.csv"); //# 2 => values: 200k -> 400k
    set3 = readCSV("data/dataset3.csv"); //# 3 => values: 400k -> 600k
    set4 = readCSV("data/dataset4.csv"); //# 4 => values: 600k -> 800k
    set5 = readCSV("data/dataset5.csv"); //# 5 => values: 800k -> 1M
    sets = { Set(set1, 1), Set(set2, 2), Set(set3, 3), Set(set4, 4), Set(set5, 5), };
    fullset = mergeVectors(set1,set2,set3,set4,set5);
}

void import_Benchmarking_data() {
    set1 = readCSV("data/benchmarking/set1.csv"); //# 1 => values: 0 -> 200k
    set2 = readCSV("data/benchmarking/set2.csv"); //# 2 => values: 200k -> 400k
    set3 = readCSV("data/benchmarking/set3.csv"); //# 3 => values: 400k -> 600k
    set4 = readCSV("data/benchmarking/set4.csv"); //# 4 => values: 600k -> 800k
    set5 = readCSV("data/benchmarking/set5.csv"); //# 5 => values: 800k -> 1M
    sets = { Set(set1, 1), Set(set2, 2), Set(set3, 3), Set(set4, 4), Set(set5, 5), };
    fullset = mergeVectors(set1,set2,set3,set4,set5);
}

void import_Bankblacklisting_data() {
    set1 = readCSV("data/blacklist/set1.csv"); //# 1 => values: 0 -> 200k
    set2 = readCSV("data/blacklist/set2.csv"); //# 2 => values: 200k -> 400k
    set3 = readCSV("data/blacklist/set3.csv"); //# 3 => values: 400k -> 600k
    set4 = readCSV("data/blacklist/set4.csv"); //# 4 => values: 600k -> 800k
    set5 = readCSV("data/blacklist/set5.csv"); //# 5 => values: 800k -> 1M
    sets = { Set(set1, 1), Set(set2, 2), Set(set3, 3), Set(set4, 4), Set(set5, 5), };
    fullset = mergeVectors(set1,set2,set3,set4,set5);
}

// Store Type Constructors as a List of Lambdas
using TypeCreator = std::function<std::shared_ptr<AbstractFilter>()>;

// Function to create a list of lists of instances
std::vector<std::vector<std::shared_ptr<AbstractFilter>>> iterateTypes(
    const std::vector<TypeCreator>& typeCreators, int instancesPerType) 
{
    std::vector<std::vector<std::shared_ptr<AbstractFilter>>> storage;
    for (const auto& creator : typeCreators) {
        std::vector<std::shared_ptr<AbstractFilter>> instanceList;
        for (int i = 0; i < instancesPerType; ++i) {
            instanceList.push_back(creator());
        }
        storage.push_back(std::move(instanceList));
    }

    return storage; // Return list of lists
}

class Tester {
public:
    shared_ptr<BloomFilterModels::AbstractFilter> bf;
    vector<shared_ptr<TestCase>> tcs; // list of test case for different configurations in one run
    
    vector<string> keys; // flattened key sets
    vector<int> keysets; // indexes of every key_set in sets

    vector<string> nonkeys; // flattened non key sets
    double binsearch_operatetime;

    Tester(std::shared_ptr<BloomFilterModels::AbstractFilter> bf) : bf(std::move(bf)) {
        if (this->bf != nullptr) cout << "Tester object created!" << endl;
    }

#ifdef OBSOLETE
    // keys: set1, set2, set3, set4 || nonkeys: set5
    void initTester800() {
        auto data = mergeVectors(set1, set2, set3, set4);
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);  
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        cout << "[INFO] BinarySearchWrite: Inserted " << data.size() << " elements. Time: " << binsearch_operatetime << "s\n";
        this->nonkeys = set5;
        this->getEntrySize();
    }

    // keys: set1 || nonkeys: set2, set3, set4
    void initTester200() {
        auto data = mergeVectors(set1);
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        cout << "[INFO] BinarySearchWrite: Inserted " << data.size() << " elements. Time: " << binsearch_operatetime << "s\n";
        this->nonkeys = mergeVectors(set2, set3, set4);
        this->getEntrySize();
    }

    // keys: set1, set2 || nonkeys: set3, set4
    void initTester400() {
        auto data = mergeVectors(set1, set2);
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        cout << "[INFO] BinarySearchWrite: Inserted " << data.size() << " elements. Time: " << binsearch_operatetime << "s\n";
        this->nonkeys = mergeVectors(set3, set4);
        this->getEntrySize();
    }

    // keys: set1, set2 || nonkeys: set3, set4
    void initTester600() {
        auto data = mergeVectors(set1, set2, set3 );
        auto start = chrono::high_resolution_clock::now();
        for (auto item : data) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        binsearch_operatetime = chrono::duration<double>(end-start).count();
        cout << "[INFO] BinarySearchWrite: Inserted " << data.size() << " elements. Time: " << binsearch_operatetime << "s\n";
        this->nonkeys = mergeVectors( set4, set5 );
        this->getEntrySize();
    }

#endif

    string getKeySet() {
        string res="";
        for (int i=0; i<sets.size(); i++) {
            res += sets[i].isKey? to_string(i) + ",":"";
        }
        if (!res.empty()) res.pop_back();
        return res;
    }

    string getNonkeySet() {
        string res="";
        for (int i=0; i<sets.size(); i++) {
            res += sets[i].isKey == false ? to_string(i)+",":"";
        }
        if (!res.empty()) res.pop_back();
        return res;
    }
    
    /*
    `s`: is for number of sets need to be key set.
    Change make to `keys`, `nonkeys`, `sets[].isKey`
    */
    void initTesterwNKSet(int s) { 
        if (sets.empty() || sets.size()==0) {
            cout << "[ERROR] There's nothing in your data set.\n";
            throw std::invalid_argument("Set is empty.");
        }
        if (s > sets.size()) {
            cout << "[ERROR] Sets cannot handle so many instances of static filters.\n";
            throw std::invalid_argument("Key set size is wrongly set.");
        }
        
        // Setup once, maybe outside the function or as static
        std::random_device rd;  // Non-deterministic random source (if available)
        std::mt19937 gen(rd()); // Mersenne Twister PRNG
        std::uniform_int_distribution<> dist(0, sets.size() - 1);
        
        while (keysets.size() < s) {
            auto index = dist(gen);
            if (find(keysets.begin(),keysets.end(),index) == keysets.end())
            {
                keysets.push_back(index);
                sets[index].isKey = true; 
            }
        }
        cout << "[initTester] Done randomly selecting sets to be key sets.\n";
        for (auto set : sets) {
            if (set.isKey) {
                binsearch_operatetime = 0;
                auto start = chrono::high_resolution_clock::now();
                for (auto item : set.data) keys.push_back(item);
                sort(keys.begin(), keys.end());
                auto end = chrono::high_resolution_clock::now(); 
                binsearch_operatetime += chrono::duration<double>(end-start).count();
            } else {
                for (auto item : set.data) nonkeys.push_back(item);
            }
        }
        cout << "[initTester] Done adding "<< keys.size() <<" to db for BSearch.\n";
    }

    void resetKeyAndSet() {
        for (int i=0; i<sets.size(); i++) {sets[i].isKey = false;}
        keys.clear(); nonkeys.clear(); keysets.clear();
    }

#ifdef OBSOLETE
    void getEntrySize() {
        cout << "Keys: " << keys.size() << endl;
        cout << "NonKeys: " << nonkeys.size() << endl;
    }

    string getConfig() {
        return bf->getConfigure();
    }

    // Return capacity of bf
    string Capacity() {
        return to_string(bf->Capacity());
    }

    // Return number of hash functions
    uint32_t HashFunctionCount() {
        return bf->K();
    }
#endif
    // Function: Insert and Sort
    void BinarySearchWrite(vector<string> dataarr) {
        
        auto start = chrono::high_resolution_clock::now();
        for (const auto& item : dataarr) {
            keys.push_back(item);
        }
        auto end = chrono::high_resolution_clock::now();
        
        // * removing sorting cost because as we simulating db flow, new indexes will not cause sorting accrossthe index table but added in constant time with B-tree
        // Sorting based on extracted ID
        sort(keys.begin(), keys.end(), [](const string& a, const string& b) {
            return extractID(a) < extractID(b);
        });
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
        
        unordered_set<string> toRemove;  
        auto start = chrono::high_resolution_clock::now();
        for (const auto& item : subtractArray) {
            toRemove.insert(extractID(item));  // Store IDs only
        }

        auto it = remove_if(keys.begin(), keys.end(), [&toRemove](const string& key) {
            return toRemove.count(extractID(key)) > 0;
        });

        keys.erase(it, keys.end());
        auto end = chrono::high_resolution_clock::now();

        // Sorting after deletion
        sort(keys.begin(), keys.end(), [](const string& a, const string& b) {
            return extractID(a) < extractID(b);
        });

        binsearch_operatetime = chrono::duration<double>(end - start).count();

        cout << "[INFO] BinarySearchRemove: Removed " << subtractArray.size() << " elements. Time: " << binsearch_operatetime << "s\n";
    }

#ifdef OBSOLETE
    // Input: vector of strings || Output: chrono::duration<double> as total time elapsed
    // Reset the filter then insert
    chrono::duration<double> testAdding(vector<string> dataArray) {
        chrono::duration<double> total_elapsed(0);
        cout << "Testing Adding of "<< dataArray.size() <<" keys!" << endl;

        if (bf->getFilterName() == "XorFilter") {
            vector<vector<uint8_t>> data;
            for (auto d : dataArray) {
                data.push_back(getAsciiBytes(d));
            }
            auto start = chrono::high_resolution_clock::now();
            bf->Init(data);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        } else {
            if (BloomFilterModels::StaticFilter* sf = dynamic_cast<BloomFilterModels::StaticFilter*>(bf.get())) {
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
                bf->Add(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
                // cout << "Adding: " << dataBytes.data() << endl;
            }
            
            } else {
// * Dynamic Filter will need this to erradicate 1 key exist accoss multiple filter (no matter if it is just an FP or not)
            for (auto data : dataArray) {
                vector<uint8_t> dataBytes = getAsciiBytes(data);
                auto start = chrono::high_resolution_clock::now();
                bf->TestAndAdd(dataBytes);
                auto end = chrono::high_resolution_clock::now();
                total_elapsed += end - start;
            }
        }
        return total_elapsed;
        }
    }

    chrono::duration<double> testInserting(vector<string> dataArray) {
        chrono::duration<double> total_elapsed(0);
        cout << "Testing Inserting of "<< dataArray.size() <<" keys!" << endl;

        for (auto data : dataArray) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            auto start = chrono::high_resolution_clock::now();
            bf->Add(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
            // cout << "Adding: " << dataBytes.data() << endl;
        }
        return total_elapsed;
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
            bf->Test(dataBytes);
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
            bf->TestAndRemove(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        }
        return total_elapsed;
    }

#endif 

    Result testInserting_v1(vector<string> dataArray) {
        Result res;
        res.nof_collision = 0;
        chrono::duration<double> total_elapsed(0);
        cout << "Testing Inserting of "<< dataArray.size() <<" keys!" << endl;

        for (auto data : dataArray) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            res.nof_collision += bf->Test(dataBytes);
            auto start = chrono::high_resolution_clock::now();
            bf->Add(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
            // cout << "Adding: " << dataBytes.data() << endl;
        }

        res.testCount = dataArray.size();
        res.elapsed = total_elapsed;
        return res;
    }

    Result testAdding_v1(string algo = Defaults::HASH_ALGORITHM, string scheme = Defaults::HASH_SCHEME) {
        Result res;
        res.nof_collision = 0;

        chrono::duration<double> total_elapsed(0);
        cout << "Testing Adding of "<< keys.size() <<" keys!" << endl;

        if (bf->getFilterName() == "XorFilter") {
            vector<vector<uint8_t>> data;
            for (auto d : keys) {
                data.push_back(getAsciiBytes(d));
            }
            auto start = chrono::high_resolution_clock::now();
            bf->Init(data);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        }
        if (BloomFilterModels::StaticFilter* sf = dynamic_cast<BloomFilterModels::StaticFilter*>(bf.get())) {
            auto start = chrono::high_resolution_clock::now();
            sf->Init(
                keys.size(), Defaults::BUCKET_SIZE, Defaults::FALSE_POSITIVE_RATE, 
                0, 0, algo, scheme
            );
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        
        for (auto data : keys) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            auto start = chrono::high_resolution_clock::now();
            bool testBf = bf->Test(dataBytes);
            if (!testBf) bf->Add(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            res.nof_collision += testBf ;
            total_elapsed += end - start;
            // cout << "Adding: " << dataBytes.data() << endl;
        }
        
        } else {
    // * Dynamic Filter will need this to erradicate 1 key exist accoss multiple filter (no matter if it is just an FP or not)
        for (auto data : keys) {
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            res.nof_collision += bf->Test(dataBytes) ;
            auto start = chrono::high_resolution_clock::now();
            bf->TestAndAdd(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
            }
        }
    
        res.elapsed = total_elapsed;
        res.testCount = keys.size();
        return res;
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
            res.nof_removable += bf->TestAndRemove(dataBytes);
            auto end = chrono::high_resolution_clock::now();
            total_elapsed += end - start;
        }
        
        res.elapsed = total_elapsed;
        res.testCount = dataArray.size();
        return res;
    }

    Result TestFP(vector<string> dataArray, bool correctAns=true) {
        cout << "[testFP] Starting to testFP on an array of " <<dataArray.size()<<" items.\n";
        Result result;
        
        for (auto data : dataArray) {
            auto start = chrono::high_resolution_clock::now();
            
            vector<uint8_t> dataBytes = getAsciiBytes(data);
            bool testResult = bf->Test(dataBytes);  // Likely spot of the crash
            
            auto end = chrono::high_resolution_clock::now();
            
            if (testResult != correctAns && testResult == true) result.FP.push_back(data);
            if (testResult != correctAns && testResult == false) result.FN.push_back(data);
            result.elapsed += end - start;
        }
        result.finalize(dataArray.size());
            
        cout << "[testFP] Done testing .\n";
        return result;
    }
    

#ifdef OBSOLETE

    void Testsuite200keys() {}
    void Testsuite400keys() {}
    void Testsuite800keys() {}
    
    string AAdd600k (
        vector<string> skey1 = set1, 
        vector<string> skey2 = set2, 
        vector<string> skey3 = set3, 
        vector<string> snkey1 = set4,
        vector<string> snkey2 = set5
    ) {}
    
    string AAdd400k (
            vector<string> skey1 = set1, 
            vector<string> skey2 = set2, 
            vector<string> snkey1 = set3, 
            vector<string> snkey2 = set4,
            vector<string> snkey3 = set5
    ) {}
#endif

    void Std_testsuite(std::ofstream &perf, std::ofstream &conf)
    {
        // Write headers to CSV files
        TestCase tc;
        perf << tc.getHeader();

        this->initTesterwNKSet(3);
        cout << endl;

        // # Test node 1
        // Insert set S -> time
        cout << "BEFORE ADDING" << endl;
        auto res = testAdding_v1();
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

        result_1 = TestFP(set1, true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set2, true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set3, true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set4, false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set5, false);
        fcount += result_1.FP.size();
        tc.f5 = result_1.FP.size();
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
        tc.filterID = bf->getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString();
        tc.reset();
        // # END ReadTimeMENT

        // # Test node 2
        // Remove set R -> time
        res = testRemove_v1(set1);
        BinarySearchRemove(set1);
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

        result_1 = TestFP(set1, false);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set2, true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set3, true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set4, false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set5, false);
        fcount += result_1.FP.size();
        tc.f5 = result_1.FP.size();
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
        tc.filterID = bf->getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString();
        tc.reset();
        // # END ReadTimeMENT

        // # Test node 3
        // Insert set S -> time
        res = testInserting_v1(set1);
        BinarySearchWrite(set1);
        elapsed = res.elapsed.count();
        cout << "Inserting 1 Set Elapsed time: " << elapsed << "s" << endl;
        tc.adding_time = elapsed;
        tc.nof_collision = res.nof_collision;
        tc.nof_operand = res.testCount;

        // # ReadTimeMENT
        fcount = 0;
        testCount = 0;
        time = chrono::duration<double>(0);

        result_1 = TestFP(set1, true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set2, true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set3, true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set4, false);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set5, false);
        fcount += result_1.FP.size();
        tc.f5 = result_1.FP.size();
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
        tc.filterID = bf->getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString();
        tc.reset();
        // # END ReadTimeMENT

        // # Test node 4
        // Insert set S -> time
        res = testInserting_v1(set5);
        BinarySearchWrite(set5);
        elapsed = res.elapsed.count();
        cout << "Inserting Set 4 Elapsed time: " << elapsed << "s" << endl;
        tc.adding_time = elapsed;
        tc.nof_collision = res.nof_collision;
        tc.nof_operand = res.testCount;

        // # ReadTimeMENT
        fcount = 0;
        testCount = 0;
        time = chrono::duration<double>(0);

        result_1 = TestFP(set1, true);
        fcount += result_1.FP.size();
        tc.f1 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set2, true);
        fcount += result_1.FP.size();
        tc.f2 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set3, true);
        fcount += result_1.FP.size();
        tc.f3 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set4, true);
        fcount += result_1.FP.size();
        tc.f4 = result_1.FP.size();
        testCount += result_1.testCount;
        time += result_1.elapsed;

        result_1 = TestFP(set5, false);
        fcount += result_1.FP.size();
        tc.f5 = result_1.FP.size();
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
        tc.filterID = bf->getFilterCode();

        // Write performance data to the CSV
        perf << tc.toCSVString();
        tc.reset();
        // # END ReadTimeMENT

        Configuration cf(bf.get());
        conf << cf.getHeader();
        cf.bs_memory = cf.bs_memory = keys.capacity() * (sizeof(std::string) + keys[0].capacity()) * 8;
        conf << cf.toCSVString();

        // # Finalization

        cout << "Test done running!" << endl;

        perf.close();
        conf.close();
    }

    void doTestAndLog(TestCase &tc)
    {
        long long int fcount = 0;
        long long int testCount = 0;
        Result result_1;
        auto time = chrono::duration<double>(0);

        for (int i=0; i<sets.size(); i++)
        {
            auto set = sets[i]; cout << "[testFP] Testing on Set "<<i<<" isKey="<<set.isKey<<".\n";
            result_1 = TestFP(set.data, set.isKey);
            fcount += result_1.FP.size();
            tc.fp += result_1.FP.size();
            testCount += result_1.testCount;
            time += result_1.elapsed; cout << "[testFP] Set "<<i<<" fp="<<result_1.FP.size()<<" fn="<<result_1.FN.size()<<".\n";
        }

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
        tc.test_case = "InsertFullKey";
        // tc.adding_time = elapsed;
        tc.key_set = getKeySet();
        tc.nonkey_set = getNonkeySet();
        tc.test_size = testCount;
        tc.accuracy = accuracy;
        tc.test_time = time.count();
        tc.operation_time = binsearch_operatetime;
        tc.binsearch_time = bisearch_time;
        tc.filterID = bf->getFilterCode();
    }

    void testOnlyTestsuite(string algo = Defaults::HASH_ALGORITHM, string scheme = Defaults::HASH_SCHEME)
    {
        // Write headers to CSV files
        auto& tc = *tcs.back();
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        tc.id = millisec;
        cout << "[INFO] Algorithm, Scheme and Clock all set up and ready.\n";

        // # Evaluate for FP and Latency
        doTestAndLog(tc);
        cout << "[Testsuite] Done FP testing and collecting.\n";
        
        cout << "[Testsuite] Done exporting configuration.\n";

        // # Finalization
        cout << "Test done running!" << endl;
    }

    void doAddAndLog(string algo = Defaults::HASH_ALGORITHM, string scheme = Defaults::HASH_SCHEME, uint32_t step_size = 200000)
    {
        // # Test node 1
        // Insert set S -> time
        auto& tc = *tcs.back();
        
        UniversalSet uniset(getShuffledFullsetCopy(fullset)); //* shuffled version of fullset is used
        Result res; ostringstream acc_log;
        res.nof_collision = 0;
        chrono::duration<double> insert_elapsed(0);
        double test_elapsed = 0;
        uint32_t step_num = keys.size() / step_size;

        //* iterating throuhgh the set continously with drawing new data and re-evaluate
        for (int stepping=0; stepping<step_num; stepping++) 
        {
        // # Insert by step_size
            //* See if it is a StaticFilter
            if (auto sf = dynamic_cast<BloomFilterModels::StaticFilter*>(bf.get())) {
                if ( sf && !sf->getInitStatus() ) { // * Init if haven't
                cout << "First time initialization." << endl;
                    auto start = chrono::high_resolution_clock::now();
                    sf->Init(
                        keys.size(), Defaults::BUCKET_SIZE, Defaults::FALSE_POSITIVE_RATE, 
                        0, 0, algo, scheme
                    );
                    auto end = chrono::high_resolution_clock::now();
                    insert_elapsed += end - start;
                }
            
                cout << "Adding to static filter : " <<uniset.keyEnd-uniset.keyStart+1<< endl;
                for (auto data : uniset.withDraw(step_size)) {
                    vector<uint8_t> dataBytes = getAsciiBytes(data);
                    bool testBf = bf->Test(dataBytes);
                    auto start = chrono::high_resolution_clock::now();
                    bf->Add(dataBytes);
                    auto end = chrono::high_resolution_clock::now();
                    res.nof_collision += testBf ;
                    insert_elapsed += end - start;
                }
                
            } else {
                cout << "Adding to dynamic filter : " <<uniset.keyEnd-uniset.keyStart+1<< endl;
        // * Dynamic Filter will need this to erradicate 1 key exist accoss multiple filter (no matter if it is just an FP or not)
                for (auto data : uniset.withDraw(step_size)) {
                    vector<uint8_t> dataBytes = getAsciiBytes(data);
                    res.nof_collision += bf->Test(dataBytes) ;
                    auto start = chrono::high_resolution_clock::now();
                    bf->TestAndAdd(dataBytes);
                    auto end = chrono::high_resolution_clock::now();
                    insert_elapsed += end - start;
                }
            }

        // # Test by step_size
            long long int fcount = 0;
            long long int testCount = 0;
            Result result;
            auto test_subelapsed = chrono::duration<double>(0);

            cout << "[testFP] Testing on indexStart="<<uniset.keyStart<<" indexEnd="<<uniset.keyEnd<<" .\n";
            result = TestFP(uniset.getKeys(), true);
            fcount += result.FP.size();
            tc.fp += result.FP.size();
            testCount += result.testCount;
            test_subelapsed += result.elapsed; cout << "[testFP] Set keys fp="<<result.FP.size()<<" fn="<<result.FN.size()<<".\n";
            
            cout << "[testFP] Testing on indexStart="<<uniset.keyEnd+1<<" .\n";
            result = TestFP(uniset.getNonKeys(), false);
            fcount += result.FP.size();
            tc.fp += result.FP.size();
            testCount += result.testCount;
            test_subelapsed += result.elapsed; cout << "[testFP] Set nonkeys fp="<<result.FP.size()<<" fn="<<result.FN.size()<<".\n";

            // float accuracy = 1.0f - static_cast<float>(fcount) / static_cast<float>(testCount);
            // : save the accuracy log set. FP - key list size - universe list size
            test_elapsed += test_subelapsed.count() / uniset.elements.size();
            acc_log << "(" << fcount << " " << uniset.getKeys().size() << " " << uniset.elements.size() << ")";
        }
        
        tc.adding_time = insert_elapsed.count() / uniset.getKeys().size();
        tc.test_time = test_elapsed / step_num;
        tc.nof_collision = res.nof_collision;
        tc.test_case = "InsertFullKey";
        tc.operation_time = binsearch_operatetime;
        tc.binsearch_time = BinarySearchReadTime(fullset).count();
        tc.filterID = bf->getFilterCode();
        tc.accuracy_log = acc_log.str();
    }

    void BusEval(vector<string> hashFuncs, vector<string> hashSchemes, int sets4test = 3) 
    {
        initTesterwNKSet(sets4test);
        cout << "Key Set is : "<< getKeySet() <<endl;

        for (auto hashFunc:hashFuncs) { for (auto hashScheme : hashSchemes) {
            auto tc = make_shared<TestCase>(); tc->algo=hashFunc; tc->scheme=hashScheme; tc->filterName=bf->getFilterCode();
            tc->bs_memory = keys.capacity() * (sizeof(std::string) + keys[0].capacity()) * 8;
            tcs.push_back(tc);
            
            cout << "\n[BEGIN] Filter: " << bf->getFilterCode() << endl;
            cout << "Configuration : "<< tc->algo <<"__"<< tc->scheme <<"__"<< tc->filterName << endl;
            
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = now.time_since_epoch();
            auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(); tc->id = millisec;
            
            doAddAndLog(hashFunc, hashScheme);
            tc->bf_memory=bf->Size();
        
            cout << "[Testsuite] Done for 1 test case .\n";
        } }

        // resetKeyAndSet();
        // cout << "Key Size ="<< to_string(keys.size()) <<".\n";
        // cout << "[INFO] Evaluation and Resetting sets done.\n";
    }

    void BankEval(std::vector<TypeCreator> &typeList, int instancesPerType, vector<string> hashFuncs, vector<string> hashSchemes)
    {
        auto filterSets = iterateTypes(typeList, instancesPerType);
        
        // * number of children filter should be equal to that of number of key sets
        initTesterwNKSet(instancesPerType);
        cout << "Key Set is : "<< getKeySet() <<endl;
        
        for (auto hashFunc : hashFuncs) { for (auto hashScheme : hashSchemes) { for (auto filterSet : filterSets) {
            // * each run is with a different combination of [StaticFilter x Algo x Scheme]
            // * loop through the StaticFilters and initialize them
            // * bank's data -> blacklisted accounts

            // * initiating the testing filter if it is MCBF
            bool isMCBF = bf->getFilterCode() == "MergCBF";
            // cout << "[LOG] " << filterSets.size() <<endl;
            if (isMCBF) bf->Init(keys.size(), Defaults::BUCKET_SIZE, Defaults::FALSE_POSITIVE_RATE, 0, 0, hashFunc, hashScheme);
            
            double add_time = 0; double merg_time = 0;
            for (int i = 0; i < instancesPerType; i++)
            {
                int key_index = keysets[i]; auto _filter = filterSet[i];
                cout << "Adding Key set is "<< key_index <<endl;
                
                auto start_add = chrono::high_resolution_clock::now();
                _filter->Init(isMCBF? keys.size():sets[key_index].data.size(),
                    Defaults::BUCKET_SIZE, Defaults::FALSE_POSITIVE_RATE, 0, 0, hashFunc, hashScheme);
                
                // * insert the child filter with key data
                for (auto item : sets[key_index].data)
                {
                    // cout << "Adding "<< item <<" to "<<_filter->getFilterCode()<<endl;
                    auto item_ = getAsciiBytes(item);
                    _filter->Add(item_);
                }

                auto end_add = chrono::high_resolution_clock::now();
                add_time += chrono::duration<double>(end_add-start_add).count();
                
                auto start_merg = chrono::high_resolution_clock::now();
                // * add them to their parent
                bf->AddFilter(_filter);
                auto end_merg = chrono::high_resolution_clock::now();
                merg_time += chrono::duration<double>(end_merg-start_merg).count();
            }
            auto tc = make_shared<TestCase>(); 
            tcs.push_back(tc);
            
            cout << "\n[BEGIN] Filter: " << bf->getFilterCode() << endl;
            cout << "Configuration : "<< hashFunc <<"__"<< hashScheme <<"__"<< bf->getFilterCode() << endl;
            testOnlyTestsuite(hashFunc, hashScheme);
            tc->algo=hashFunc; tc->scheme=hashScheme; tc->filterName=bf->getFilterCode(); tc->bf_memory=bf->Size(); tc->adding_time=add_time+merg_time; tc->merge_time=merg_time;
            tc->bs_memory = keys.capacity() * (sizeof(std::string) + keys[0].capacity()) * 8;
            // cout << "Config of filter: " << bf->getConfigure() << endl;
            bf->Reset();
            cout << "Testcase is DONE.\n";
        } } }

        resetKeyAndSet();
        cout << "[INFO] Evaluation and Resetting sets done.\n";
    }

    ~Tester() {}
};

int main(int argc, char *argv[])
{
    // string mode = "bank"; if (argc > 1) mode = argv[1];
    // if (mode == "bank") {
    //     import_Bankblacklisting_data();
    // } else if (mode == "bench") {
    //     import_Benchmarking_data();
    // } else if (mode == "bus") {
    //     import_Busticketing_data();
    // } else {
    //     import_Busticketing_data();
    // }

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
        // Hash32::SCHEME_SERIAL,
        Hash32::SCHEME_KIRSCH_MITZENMACHER,
        // Hash32::SCHEME_ENHANCED_DOUBLE_HASHING,
    };

    for (auto hashFunc : hashFuncs) {
        for (auto hashScheme : hashSchemes) {
            DeletableBloomFilter *dlbf = new DeletableBloomFilter();
            cout << "Filter: " << dlbf->getFilterCode() << endl;
            Tester tester(*dlbf);
            tester.BusEval(hashFunc, hashScheme);
            cout << "Config of filter: " << dlbf->getConfigure() << endl;
            delete(dlbf);
        }
    }
    
#endif
    
#ifdef Bus
{
    import_Busticketing_data();

    vector<string> hashFuncs = {
        // Hash32::ALGO_MURMUR3_32,
        Hash32::ALGO_MURMUR3_128,
        // Hash32::ALGO_FNV1A,
        // Hash32::ALGO_SIPHASH,
        Hash32::ALGO_SHA256
    };

    vector<string> hashSchemes = {
        // Hash32::SCHEME_SERIAL,
        Hash32::SCHEME_KIRSCH_MITZENMACHER,
        // Hash32::SCHEME_ENHANCED_DOUBLE_HASHING,
    };
    
    int instancesPerType = 1;

    vector<TypeCreator> typeList = {
        []() { return std::make_shared<OneHashingBloomFilter>(); },
        []() { return std::make_shared<DeletableBloomFilter>(); },
        []() { return std::make_shared<StandardBloomFilter>(); },
        []() { return std::make_shared<CountingBloomFilter>(); },
        // []() { return std::make_shared<ScalableDeletableBloomFilter>(); },
        // []() { return std::make_shared<ScalableStandardBloomFilter>(); },
        []() { return std::make_shared<CountingScalableBloomFilter>(); },
        // []() { return std::make_shared<DynamicBloomFilter>(); },
        // []() { return std::make_shared<DynamicStdCountingBloomFilter>(); }
    };

    vector<shared_ptr<TestCase>> tcs;
    auto filterSets = iterateTypes(typeList, 1);
    
    // Loop over configs
    for (auto filterSet : filterSets) { for (auto filter : filterSet) {
        Tester tester(filter);  // assuming Tester takes reference to BaseCompositeFilter
        tester.BusEval(hashFuncs, hashSchemes);
        tcs = mergeTcs(tcs, tester.tcs);
    } }

    logResult(tcs);
}
#endif

#ifdef Banking
{
    import_Bankblacklisting_data();

    vector<string> hashFuncs = {
        // Hash32::ALGO_MURMUR3_32,
        Hash32::ALGO_MURMUR3_128,
        Hash32::ALGO_FNV1A,
        Hash32::ALGO_SIPHASH,
        Hash32::ALGO_JENKINS
    };

    vector<string> hashSchemes = {
        // Hash32::SCHEME_SERIAL,
        Hash32::SCHEME_KIRSCH_MITZENMACHER,
        // Hash32::SCHEME_ENHANCED_DOUBLE_HASHING,
    };
    
    int instancesPerType = 3;

    std::vector<std::pair<std::vector<TypeCreator>, std::shared_ptr<BloomFilterModels::AbstractFilter>>> configs = {
        {
            {
                []() { return std::make_shared<DeletableBloomFilter>(); },
                []() { return std::make_shared<StandardBloomFilter>(); },
                []() { return std::make_shared<CountingBloomFilter>(); },
                []() { return std::make_shared<OneHashingBloomFilter>(); }
            },
            std::make_shared<BloomFilterModels::MergeableFilter>()
        }
        ,
        {
            {
                []() { return std::make_shared<MergeableCountingBloomFilter>(); }
            },
            std::make_shared<BloomFilterModels::MergeableCountingBloomFilter>()
        }
    };

    vector<shared_ptr<TestCase>> tcs;
    
    // Loop over configs
    for (auto &[typeList, filter] : configs) {
        Tester tester(filter);  // assuming Tester takes reference to BaseCompositeFilter
        tester.BankEval(typeList, instancesPerType, hashFuncs, hashSchemes);
        tcs = mergeTcs(tcs, tester.tcs);
    }

    logResult(tcs);
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
            tester.BusEval(hashFunc, hashScheme);

            delete(scbf);
        }
    }
#endif
    return 0;
}
