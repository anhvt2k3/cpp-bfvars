#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <random>

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