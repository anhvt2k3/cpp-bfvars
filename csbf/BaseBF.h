#pragma once
#include "../utils/Defaults.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels {
// ! Filter base class
    class AbstractFilter {
    public:
    // # Compulsory methods
        virtual string getFilterName() const = 0;
        virtual string getFilterCode() const = 0;
        virtual uint32_t Size() const = 0;
        virtual uint32_t Capacity() const = 0;
        virtual double FPrate() const = 0;
        virtual uint32_t K() const = 0;
        virtual uint32_t Count() const = 0;
        virtual bool Test(const std::vector<uint8_t>& data) const = 0;
        virtual AbstractFilter& Add(const std::vector<uint8_t>& data) = 0;
        // virtual AbstractFilter& Reset() = 0;
    // # Specialized methods
        virtual void Init(uint32_t n, uint8_t b = Defaults::BUCKET_SIZE, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t countExist = 0) {
            cout << "Unsupported method: Init" << endl;
        };
        virtual void Init(vector<vector<uint8_t>> data, uint32_t countExist = 0) {
            cout << "Unsupported method: Init" << endl;
        }
        virtual string getConfigure() {
            return "Unsupported method: getConfigure";
        };
        virtual bool TestAndRemove(const std::vector<uint8_t>& data){
            cout << "Unsupported method: TestAndRemove" << endl;
            return false;
        };
    };
// ! Filter parent class
    class StaticFilter : public AbstractFilter {
protected:
        unique_ptr<Buckets> buckets; // Bucket array
        uint32_t m; // Filter size (number of buckets)
        uint32_t k; // Number of hash functions
        uint32_t count; // Number of items added
        uint32_t maxCapacity; // Maximum capacity of the filter
        double fpRate; // Target false-positive rate
        
public:
        virtual void Init(uint32_t n, uint8_t b = Defaults::BUCKET_SIZE, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t countExist = 0) override {
            this->buckets = make_unique<Buckets>(BloomFilterApp::Utils::OptimalMCounting(n, fpRate), b);
            this->m = BloomFilterApp::Utils::OptimalMCounting(n, fpRate);
            this->k = BloomFilterApp::Utils::OptimalKCounting(fpRate);
            this->count = countExist;
            this->maxCapacity = n;
            this->fpRate = fpRate;
        }

        string getConfigure() {
            string res = "_ _ _ Filter Configuration _ _ _\n";
            res += "Filter Size: " + to_string(Size()) + "\n";
            res += "Capacity: " + to_string(Capacity()) + "\n";
            res += "Number of Hash Functions: " + to_string(K()) + "\n";
            res += "False Positive Rate: " + to_string(FPrate()) + "\n";
            res += "Number of Items Added: " + to_string(Count()) + "\n";
            res += "_ _ _ Buckets Configuration _ _ _\n";
            res += "Bucket Size per: " + to_string(buckets->bucketSize) + " (Max value: " + to_string(buckets->Max) + ")" "\n";
            res += "Bucket Count: " + to_string(buckets->count) + "\n";
            return res;
        }
        StaticFilter() {}
        StaticFilter(uint32_t n, uint8_t b, double fpRate, uint32_t countExist = 0) {
            this->Init(n, b, fpRate, countExist);
        }
    };

    class DynamicFilter : public AbstractFilter {};

}