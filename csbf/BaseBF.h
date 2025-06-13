#pragma once
#include "../utils/Defaults.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels
{
    // ! Filter base class
    class AbstractFilter
    {
    public:
        // : these constants only required for static filter
        unique_ptr<Buckets> buckets; // Bucket array
        uint32_t m;                  // Filter size (number of buckets)
        uint32_t k;                  // Number of hash functions
        uint32_t count;              // Number of items added
        uint32_t maxCapacity;        // Maximum capacity of the filter

        string algorithm;         // Hash algorithm to use
        string scheme;            // Hashing scheme to use (default: EDH)
        Hash32::HashGen *hashGen; // Hash generator for creating hash indices

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
        
        // virtual AbstractFilter& Reset() = 0;
        virtual bool Test(const std::vector<uint8_t> &data) const = 0;
        
                        // # Specialized methods
        uint32_t BucketMaxValue() const { return 0; };
        uint32_t BucketCount() const { return 0; };
        string getHashAlgo() const { return algorithm; };
        string getHashScheme() const { return scheme; };

        virtual AbstractFilter &Add(const std::vector<uint8_t> &data) { return *this; }

        virtual void Init(
            uint32_t n, // estimated total size of data
            uint8_t b = Defaults::BUCKET_SIZE,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            uint32_t k = 0,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME)
        {
            cout << "Unsupported method: Init" << endl;
        };
        virtual void Init(vector<vector<uint8_t>> data, uint32_t countExist = 0)
        {
            cout << "Unsupported method: Init" << endl;
        }
        virtual string getConfigure()
        {
            return "Unsupported method: getConfigure";
        };
        virtual bool TestAndRemove(const std::vector<uint8_t> &data)
        {
            cout << "Unsupported method: TestAndRemove" << endl;
            return false;
        };
        virtual bool Remove(const std::vector<uint8_t> &data)
        {
            cout << "Unsupported method: TestAndRemove" << endl;
            return false;
        };
        virtual bool TestAndAdd(const std::vector<uint8_t> &data)
        {
            cout << "Unsupported method: TestAndAdd" << endl;
            return false;
        };
        virtual int AddFilter(shared_ptr<AbstractFilter> target)
        {
            cout << "Unsupported method: AddFilter" << endl;
            return 0;
        }
        virtual AbstractFilter* Duplicate(uint32_t n, double fpRate, int k) 
        { 
            cout<<"Unsupported method: Duplicate"<<endl; 
            throw std::logic_error("This method is available in StaticFilter only!"); 
            return nullptr;
        }
        virtual AbstractFilter &Reset() { return *this; };
    };

    // ! Filter parent class
    class StaticFilter : public AbstractFilter
    {
    bool isInitted = false;
    public:
        void Init(
            uint32_t n,
            uint8_t b = Defaults::BUCKET_SIZE,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            uint32_t k = 0,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME) override
        {
            //@ automatically destroy the last buckets and assign a new one
            this->isInitted = true;
            this->m = BloomFilterApp::Utils::OptimalMCounting(n, fpRate);
            this->k = k == 0 ? BloomFilterApp::Utils::OptimalKCounting(fpRate) : k;
            this->buckets = make_unique<Buckets>(this->m, b);
            this->count = countExist;
            this->maxCapacity = n;
            this->fpRate = fpRate;
            this->algorithm = algorithm;
            this->scheme = scheme;
            this->hashGen = new Hash32::HashGen{this->k, this->m};
            cout << "[LOG] A Static filter created with m="<<this->m<<" k="<<this->k<<".\n";
        }
        void Init_(
            uint32_t n,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            uint32_t k = 0,
            uint8_t b = Defaults::BUCKET_SIZE,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME)
        {
            //@ automatically destroy the last buckets and assign a new one
            this->isInitted = true;
            this->m = BloomFilterApp::Utils::OptimalMCounting(n, fpRate);
            this->k = k == 0 ? BloomFilterApp::Utils::OptimalKCounting(fpRate) : k;
            this->buckets = make_unique<Buckets>(this->m, b);
            this->count = countExist;
            this->maxCapacity = n;
            this->fpRate = fpRate;
            this->algorithm = algorithm;
            this->scheme = scheme;
            this->hashGen = new Hash32::HashGen{this->k, this->m};
            cout << "[LOG] A Static filter created with m="<<this->m<<" k="<<this->k<<".\n";
        }

        // :TODO
        StaticFilter* Duplicate(uint32_t capacity, double fpRate, int k) {
            // : duplicate itself with some minor changes as input parameters if needed
            return nullptr;
        }

        bool getInitStatus() {
            return this->isInitted;
        }

        uint32_t BucketSize() const
        {
            return buckets->bucketSize;
        }

        uint32_t BucketMaxValue() const {
            return buckets->MaxBucketValue();
        }

        uint32_t BucketCount() const {
            return buckets->BucketCount();
        }

        string getHashAlgo() const
        {
            return this->algorithm;
        }

        string getHashScheme() const
        {
            return this->scheme;
        }

        string getConfigure()
        {
            string res = "_ _ _ Filter Configuration _ _ _\n";
            res += "Filter Size: " + to_string(Size()) + "\n";
            res += "Capacity: " + to_string(Capacity()) + "\n";
            res += "Number of Hash Functions: " + to_string(K()) + "\n";
            res += "Hash function & scheme: " + getHashAlgo() + ";" + getHashScheme() + "\n";
            res += "False Positive Rate: " + to_string(FPrate()) + "\n";
            res += "Number of Items Added: " + to_string(Count()) + "\n";
            res += "_ _ _ Buckets Configuration _ _ _\n";
            res += "Bucket Size per: " + to_string(buckets->bucketSize) + " (Max value: " + to_string(buckets->Max) + ")""\n";
            res += "Bucket Count: " + to_string(buckets->count) + "\n";
            return res;
        }
        StaticFilter() {}
        StaticFilter(
            uint32_t n,
            uint8_t b,
            double fpRate,
            uint32_t k = 0,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME)
        {
            this->Init(n, b, fpRate, k, countExist, algorithm, scheme);
        }

        ~StaticFilter()
        {
            cout << "Releasing Hash Generator" << endl;
            delete this->hashGen;
            cout << "Resetting Bucket to nullptr" << endl;
            this->buckets.reset();
        }
    };

    class DynamicFilter : public AbstractFilter
    {
    public:
        vector<shared_ptr<AbstractFilter>> filters;

        DynamicFilter() {}
        DynamicFilter(vector<shared_ptr<AbstractFilter>> filter) : filters(filter) {}

        int AddFilter(shared_ptr<AbstractFilter> filter) {
            cout << "[INFO] This method is not implemented.\n";
            return 0;
        }

        uint32_t BucketSize() const
        {
            return filters.empty() ? 0 : filters.front()->BucketSize();
        }

        DynamicFilter &Reset()
        {
            filters.clear();
            return *this;
        }
    };

    class CuckooBase : public StaticFilter
    {
    protected:
        uint32_t fsize;
        uint32_t cargosize;

    public:
        uint32_t OptimalFingerprintSize(uint32_t fp, uint32_t b)
        {
            // return (uint32_t)(log2(1/fp) + log2(2*b) + 1) ;
            return 8;
        }

        uint32_t OptimalMComputing(uint32_t n)
        {
            return n / 0.95;
        }

        virtual void Init(
            uint32_t n, // equal maxCapacity, Capacity
            uint8_t b = Defaults::Cuckoo::BUCKET_CAPACITY,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            uint32_t k = 0,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME) override
        {
            this->m = OptimalMComputing(n);
            this->k = 2;
            this->count = countExist;
            this->maxCapacity = n;
            this->fpRate = fpRate;
            this->fsize = OptimalFingerprintSize(fpRate, b);
            this->cargosize = m / b + 1;
            this->buckets = make_unique<Buckets>(cargosize * b, fsize + 1);
        }

        string getConfigure()
        {
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
        CuckooBase(
            uint32_t n,
            uint8_t b,
            double fpRate,
            uint32_t k = 0,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME)
        {
            this->Init(n, b, fpRate, k, countExist, algorithm, scheme);
        }
    };
};