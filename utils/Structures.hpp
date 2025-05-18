#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <random>
#include <functional>

using namespace std;

class Result 
{
    public:
    long long int testCount = 0;
        long long int nof_collision = 0;
        long long int nof_removable = 0;
        chrono::duration<double> elapsed;
        vector<string> FP;
        vector<string> FN;
        float accuracy = 0;
        Result() {}
        void setFP(vector<string> FP) {
            this->FP = FP;
        }
        // This function should be called after all FP is set
        void finalize(long long int size)
        {
            testCount = size;
            if (size == 0) this->accuracy = 1.0f;
            else this->accuracy = (testCount - FP.size() - FN.size()) / testCount;
        }
        ~Result() {}
};

struct Set
{
private:
    int setCode; // deprecated and should not be used explicitly
public:
    string setName = "set<>";
    vector<string> data;
    bool isKey = 0;

    Set(vector<string> data, int code=-1) : data(data), setCode(code) {
        setName = "set<" + to_string(code) + ">";
    }

    string set_code(int num) {
        setCode = num;
        setName = "set<" + to_string(num) + ">";
        return setName;
    };

};

class UniversalSet {
private:
    vector<string> elements; // Universal set of strings
    vector<string> keys;     // Stores keys
    vector<string> nonKeys;  // Stores non-keys
    size_t keyStart;        // Index of first key
    size_t keyEnd;          // Index of last key
    bool isWithdrawn;       // Flag to check if withDraw was called

public:
    // Constructor
    UniversalSet(const vector<string>& input) 
        : elements(input), keys(), nonKeys(), 
          keyStart(0), keyEnd(0), isWithdrawn(false) {}

    // Mark x elements as keys starting from the last key pivot
    void withDraw(size_t x) {
        if (x == 0) {
            throw out_of_range("Number of elements must be greater than 0");
        }
        // Set new keyStart based on previous state
        if (!isWithdrawn) {
            keyStart = 0; // First call starts at beginning
        } else {
            keyStart = keyEnd + 1; // Continue from last key
        }
        // Calculate new keyEnd (x elements from keyStart, 1-based)
        keyEnd = keyStart + x - 1;
        if (keyEnd >= elements.size()) {
            throw out_of_range("Key range exceeds universal set size");
        }
        isWithdrawn = true;

        // Update keys
        keys.clear();
        keys.assign(elements.begin() + keyStart, elements.begin() + keyEnd + 1);

        // Update nonKeys
        nonKeys.clear();
        nonKeys.insert(nonKeys.end(), elements.begin(), elements.begin() + keyStart);
        nonKeys.insert(nonKeys.end(), elements.begin() + keyEnd + 1, elements.end());
    }

    // Return keys
    vector<string> getKeys() const {
        if (!isWithdrawn) {
            throw logic_error("withDraw must be called before getting keys");
        }
        return keys;
    }

    // Return non-keys
    vector<string> getNonKeys() const {
        if (!isWithdrawn) {
            throw logic_error("withDraw must be called before getting non-keys");
        }
        return nonKeys;
    }

    // Reset key boundaries and clear keys/nonKeys
    void reset() {
        keyStart = 0;
        keyEnd = 0;
        isWithdrawn = false;
        keys.clear();
        nonKeys.clear();
    }
};