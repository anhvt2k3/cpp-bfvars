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
    size_t keyStart;        // Index of first key
    size_t keyEnd;          // Index of last key
    bool isWithdrawn;       // Flag to check if withDraw was called

public:
    // Constructor
    UniversalSet(const vector<string>& input) 
        : elements(input), keyStart(0), keyEnd(0), isWithdrawn(false) {}

    // Mark first element as start of keys and x-th as end
    vector<string> withDraw(size_t x) {
        if (x > elements.size() || x == 0) {
            throw out_of_range("Invalid index for key boundary");
        }
        keyStart = 0;              // First element is start of keys
        keyEnd = x - 1;            // x-th element (1-based) is end of keys
        isWithdrawn = true;
        return vector<string>(elements.begin() + keyStart, 
                            elements.begin() + keyEnd + 1);
    }

    // Return keys (from keyStart to keyEnd)
    vector<string> getKeys() const {
        if (!isWithdrawn) {
            throw logic_error("withDraw must be called before getting keys");
        }
        return vector<string>(elements.begin() + keyStart, 
                            elements.begin() + keyEnd + 1);
    }

    // Return non-keys (before keyStart and after keyEnd)
    vector<string> getNonKeys() const {
        if (!isWithdrawn) {
            throw logic_error("withDraw must be called before getting non-keys");
        }
        vector<string> nonKeys;
        // Add elements before keyStart
        nonKeys.insert(nonKeys.end(), elements.begin(), 
                      elements.begin() + keyStart);
        // Add elements after keyEnd
        nonKeys.insert(nonKeys.end(), elements.begin() + keyEnd + 1, 
                      elements.end());
        return nonKeys;
    }

    // Reset key boundaries and withdrawal state
    void reset() {
        keyStart = 0;
        keyEnd = 0;
        isWithdrawn = false;
    }
};