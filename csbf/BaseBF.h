#pragma once
#include "../utils/Defaults.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels {
// ! Filter base class
    class AbstractFilter {
    public:
    // : these constants only required for static filter
        unique_ptr<Buckets> buckets; // Bucket array
        uint32_t m; // Filter size (number of buckets)
        uint32_t k; // Number of hash functions
        uint32_t count; // Number of items added
        uint32_t maxCapacity; // Maximum capacity of the filter
        double fpRate; // Target false-positive rate
    // # Compulsory methods
        virtual string getFilterName() const = 0;
        virtual string getFilterCode() const = 0;
        virtual uint32_t BucketSize() const = 0;
        virtual uint32_t Size() const = 0;
        virtual uint32_t Capacity() const = 0;
        virtual double FPrate() const = 0;
        virtual uint32_t K() const = 0;
        virtual uint32_t Count() const = 0;
        virtual bool Test(const std::vector<uint8_t>& data) const = 0;
        virtual AbstractFilter& Add(const std::vector<uint8_t>& data) = 0;
        // virtual AbstractFilter& Reset() = 0;
    // # Specialized methods
        virtual void Init(uint32_t n, uint8_t b = Defaults::BUCKET_SIZE, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t k = 0, uint32_t countExist = 0) {
            cout << "Unsupported method: Init" << endl;
        };
        virtual void Init(vector<vector<uint8_t>> data, uint32_t countExist = 0) {
            cout << "Unsupported method: Init" << endl;
        }
        virtual string getConfigure() {
            return "Unsupported method: getConfigure";
        };
        virtual bool TestAndRemove(const std::vector<uint8_t>& data) {
            cout << "Unsupported method: TestAndRemove" << endl;
            return false;
        };
        virtual bool Remove(const std::vector<uint8_t>& data) {
            cout << "Unsupported method: TestAndRemove" << endl;
            return false;
        };
        virtual bool TestAndAdd(const std::vector<uint8_t>& data) {
            cout << "Unsupported method: TestAndAdd" << endl;
            return false;
        };
        virtual AbstractFilter& Reset() { return *this; };

    };

// ! Filter parent class
    class StaticFilter : public AbstractFilter {
protected:
        
public:
        virtual void Init(uint32_t n, uint8_t b = Defaults::BUCKET_SIZE, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t k = 0, uint32_t countExist = 0) override {
            //@ automatically destroy the last buckets and assign a new one
            this->buckets = make_unique<Buckets>(BloomFilterApp::Utils::OptimalMCounting(n, fpRate), b);
            this->m = BloomFilterApp::Utils::OptimalMCounting(n, fpRate);
            this->k = k==0 ? BloomFilterApp::Utils::OptimalKCounting(fpRate):k;
            this->count = countExist;
            this->maxCapacity = n;
            this->fpRate = fpRate;
        }
        uint32_t BucketSize() const {
            return buckets->bucketSize;
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
        StaticFilter(uint32_t n, uint8_t b, double fpRate, uint32_t k = 0, uint32_t countExist = 0) {
            this->Init(n, b, fpRate, k, countExist);
        }
    };

    class DynamicFilter : public AbstractFilter {
    public:
        vector<shared_ptr<StaticFilter>> filters;

        uint32_t BucketSize() const {
            return filters.empty() ? 0 : filters.front()->BucketSize();

        }

        DynamicFilter& Reset() {
            filters.clear();
            return *this;
        }
    };

    class CuckooBase : public StaticFilter {
protected:
        uint32_t fsize;
        uint32_t cargosize;
public:
        uint32_t OptimalFingerprintSize(uint32_t fp, uint32_t b) {
            // return (uint32_t)(log2(1/fp) + log2(2*b) + 1) ;
            return 8;
        }
        
        uint32_t OptimalMComputing(uint32_t n) {
            return n / 0.95;
        }
        
        virtual void Init(uint32_t n, uint8_t b = Defaults::Cuckoo::BUCKET_CAPACITY, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t k = 0, uint32_t countExist = 0) override {
            this->m = OptimalMComputing(n);
            this->k = 2;
            this->count = countExist;
            this->maxCapacity = n;
            this->fpRate = fpRate;
            this->fsize = OptimalFingerprintSize(fpRate, b);
            this->cargosize = m/b + 1;
            this->buckets = make_unique<Buckets>(cargosize*b, fsize+1);
        }

        string getConfigure() {
            string res = "_ _ _ Filter Configuration _ _ _\n";
            // res += "Filter Size: " + to_string(Size()) + "\n";
            // res += "Capacity: " + to_string(Capacity()) + "\n";
            // res += "Number of Hash Functions: " + to_string(K()) + "\n";
            // res += "False Positive Rate: " + to_string(FPrate()) + "\n";
            // res += "Number of Items Added: " + to_string(Count()) + "\n";
            // res += "_ _ _ Buckets Configuration _ _ _\n";
            // res += "Bucket Size per: " + to_string(buckets->bucketSize) + " (Max value: " + to_string(buckets->Max) + ")" "\n";
            // res += "Bucket Count: " + to_string(buckets->count) + "\n";
            return res;
        }

        CuckooBase() {}
        CuckooBase(uint32_t n, uint8_t b, double fpRate, uint32_t k = 0, uint32_t countExist = 0) {
            this->Init(n, b, fpRate, k, countExist);
        }
    };

};