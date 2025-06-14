#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"

using namespace std;
#define Hashgen

namespace BloomFilterModels {

    class StandardBloomFilter : public StaticFilter {
public:
    StandardBloomFilter() {
        cout << "StdBF created without params.\n";
    }
    StandardBloomFilter(uint32_t n, uint8_t b, double fpRate, uint32_t k = 0, uint32_t countExist = 0, string algorithm = Defaults::HASH_ALGORITHM, string scheme = Defaults::HASH_SCHEME) 
    : StaticFilter(n, b, fpRate, k, countExist, algorithm, scheme)  // Call the base class constructor directly
    {
        cout << "Standard BF created with maxCapacity="<<n<<".\n";
    }

    void Init(uint32_t n, uint8_t b = 1, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t k = 0, uint32_t countExist = 0, string algorithm = Defaults::HASH_ALGORITHM, string scheme = Defaults::HASH_SCHEME)  {
        StaticFilter::Init(n, 1, fpRate, k, countExist, algorithm, scheme);
    }

    string getFilterName() const {
        return "StandardBloomFilter";
    }

    string getFilterCode() const {
        return "StdBF";
    }

    // Returns the maximum capacity of the filter
    uint32_t Capacity() const {
        return maxCapacity;
    }

    // Returns the filter capacity
    uint32_t Size() const {
        return m;
    }

    // Returns the number of hash functions
    uint32_t K() const {
        return k;
    }

    // Returns the number of items in the filter
    uint32_t Count() const {
        return count;
    }

    // Returns the target false-positive rate
    double FPrate() const {
        return fpRate;
    }

    shared_ptr<AbstractFilter> Duplicate(uint32_t capacity, double fpRate, int k) {
        // Duplicate itself with some minor changes as input parameters if needed
        auto newFilter = make_shared<StandardBloomFilter>(capacity, Defaults::BUCKET_SIZE, fpRate, k);
        newFilter->ResetHashing(algorithm, scheme);
        return newFilter;
    }

    #ifdef Hashgen
        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        StandardBloomFilter& Add(const std::vector<uint8_t>& data) {
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);
            // Set the K bits in the bucket array
            for (auto h : hashes) {
                // cout << "cbf-Adding: " << uint32_t((lower + upper * i) % m) << endl;
                buckets->Set(uint32_t(h), 1);
            }
    
            this->count++;
            return *this;
        }

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);
            // Check if all hash function indices are set in the bucket array
            for (auto h : hashes) {
                if (buckets->Get(uint32_t(h)) == 0)
                {
                    return false;
                }
            }

            return true;
        }
        
    #else
        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Check if all hash function indices are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                if (buckets->Get(uint32_t((lower + upper * i) % m)) == 0)
                {
                    return false;
                }
            }

            return true;
        }
        
        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        StandardBloomFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                // cout << "cbf-Adding: " << uint32_t((lower + upper * i) % m) << endl;
                buckets->Set(uint32_t((lower + upper * i) % m), 1);
            }

            this->count++;
            return *this;
        }
        
        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            bool member = true;

            // Check if all hash function indices are set in the bucket array and set them if not
            for (uint32_t i = 0; i < k; ++i) {
                uint32_t index = uint32_t((lower + upper * i) % m);
                if (buckets->Get(index) == 0) {
                    member = false;
                }
                buckets->Set(index, 1);
            }

            count++;
            return member;
        }
    #endif

        ~StandardBloomFilter() {}
    };
    

}