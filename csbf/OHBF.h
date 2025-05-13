#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"
#include "../utils/primes/primes.h"

using namespace std;
#define Hashgen

namespace BloomFilterModels {

    class PartitionModule {
public:
    int mf;
    std::vector<int> partLen;

    PartitionModule() : mf(0) {}

    // Function to find the index of the prime closest to mp/k
    int findClosestPrimeIndex(int target) {
        auto it = std::lower_bound(primes.begin(), primes.end(), target);
        if (it == primes.end()) return primes.size() - 1;
        return it - primes.begin();
    }

    vector<int> determinePartitionSizes(int mp, int k) {
        this->partLen.resize(k);

        // Step 1: Find the prime closest to mp/k and its index in pTable
        int pdex = findClosestPrimeIndex(mp / k);

        // Step 2: Initialize sum and mf
        int sum = 0;
        this->mf = 0;

        // Step 3-5: Sum primes from pTable[pdex - k + 1] to pTable[pdex]
        for (int i = pdex - k + 1; i <= pdex; ++i) {
            sum += primes[i];
        }

        // Step 6: Compute initial difference dif1 and prepare for adjustment
        int dif1 = mp - sum;
        int j = pdex + 1;

        // Step 7-8: Adjust sum and compute dif2
        sum = sum + primes[j] - primes[j - k];
        int dif2 = mp - sum;

        // Step 9-13: Continue adjusting sum while dif2 < dif1
        while (dif2 < dif1) {
            dif1 = dif2; ++j;
            sum = sum + primes[j] - primes[j - k];
            dif2 = std::abs(sum - mp);
        }

        // Step 14-17: Store the partition lengths in partLen and compute mf
        vector<int> indexes = {0};
        for (int i = 0; i < k; ++i) {
            this->partLen[i] = primes[j - k + i +1];
            this->mf += this->partLen[i];
            indexes.push_back(this->mf);
        }

        return indexes;
    }
};

    class OneHashingBloomFilter : public StaticFilter {

        vector<int> indexes;
public:
        OneHashingBloomFilter() {
            cout << "OHBF without params.\n";
        }
        OneHashingBloomFilter(uint32_t n, uint8_t b, double fpRate, uint32_t k = 0, uint32_t countExist = 0, string algorithm = Defaults::HASH_ALGORITHM, string scheme = Defaults::HASH_SCHEME) 
            : StaticFilter(n, b, fpRate, k, countExist, algorithm, scheme)  // Call the base class constructor directly
        {
            this->k *= 1.5;
            this->indexes = PartitionModule().determinePartitionSizes(this->m, this->k);
            this->hashGen->setK(1);
        }
        // Intended to be used if previously no-params constructor is used.
        void Init(uint32_t n, uint8_t b = 1, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t k = 0, uint32_t countExist = 0, string algorithm = Defaults::HASH_ALGORITHM, string scheme = Defaults::HASH_SCHEME)  {
            StaticFilter::Init(n, 1, fpRate, k, countExist, algorithm, scheme);
            this->k *= 1.5;
            this->indexes = PartitionModule().determinePartitionSizes(this->m, this->k);
            this->hashGen->setK(1);
        }

        string getFilterName() const {
            return "OneHashingBloomFilter";
        }

        string getFilterCode() const {
            return "OHBF";
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
    #ifdef Hashgen
    // Tests for membership of the data.
    // Returns true if the data is probably a member, false otherwise.
    bool Test(const std::vector<uint8_t>& data) const {
        vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

        // Check if all hash function indices are set in the bucket array
        for (uint32_t i = 0; i < k; ++i) {
            int mfi = indexes[i+1] - indexes[i];
            uint32_t indx = uint32_t(hashes[0] % mfi + indexes[i]);
            if (buckets->Get(indx) == 0)
            {
                return false;
            }
        }

        return true;
    }

    // Adds the data to the filter->
    // Returns a reference to the filter for chaining.
    OneHashingBloomFilter& Add(const std::vector<uint8_t>& data) {
        vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);
        
        // Set the K bits in the bucket array
        for (uint32_t i = 0; i < k; ++i) {
            int mfi = indexes[i+1] - indexes[i];
            uint32_t indx = uint32_t(hashes[0] % mfi + indexes[i]);
            buckets->Set(indx, 1);
        }

        this->count++;
        return *this;
    }
    #else
        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "single"); // Generate hash kernels
            uint32_t hv = hashKernel.LowerBaseHash;

            // Check if all hash function indices are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                int mfi = indexes[i+1] - indexes[i];
                uint32_t indx = uint32_t(hv % mfi + indexes[i]);
                if (buckets->Get(indx) == 0)
                {
                    return false;
                }
            }

            return true;
        }

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        OneHashingBloomFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "single"); // Generate hash kernels
            uint32_t hv = hashKernel.LowerBaseHash;

            // Set the K bits in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                int mfi = indexes[i+1] - indexes[i];
                uint32_t indx = uint32_t(hv % mfi + indexes[i]);
                buckets->Set(indx, 1);
            }

            this->count++;
            return *this;
        }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "single"); // Generate hash kernels
            uint32_t hv = hashKernel.LowerBaseHash;
            bool member = true;

            // Check if all hash function indices are set in the bucket array and set them if not
            for (uint32_t i = 0; i < k; ++i) {
                int mfi = indexes[i+1] - indexes[i];
                uint32_t indx = uint32_t(hv % mfi + indexes[i]);
                if (buckets->Get(indx) == 0) {
                    member = false;
                }
                buckets->Set(indx, 1);
            }

            count++;
            return member;
        }
    #endif
        ~OneHashingBloomFilter() {}
    };


}