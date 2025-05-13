#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels
{
    class DeletableBloomFilter : public StaticFilter
    {
        unique_ptr<Buckets> collisionFree; // Bucket array
    public:
        DeletableBloomFilter()
        {
            cout << "DBF constructor without params" << endl;
        }
        DeletableBloomFilter(
            uint32_t n,
            uint8_t b = 1,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            uint32_t k = 0,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME)
            : StaticFilter(
                  n,
                  b,
                  fpRate,
                  k,
                  countExist,
                  algorithm,
                  scheme) // Call the base class constructor directly
        {
            cout << "DBF constructor with params" << endl;
            uint32_t m = uint32_t(BloomFilterApp::Utils::OptimalMCounting(n, fpRate) / Defaults::COLLIDE_REGION_SIZE);
            collisionFree = make_unique<Buckets>(m, 1);
        }

        void Init(
            uint32_t n,
            uint8_t b = 1,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            uint32_t k = 0,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME) override
        {
            StaticFilter::Init(n, 1, fpRate, k, countExist, algorithm, scheme);
            uint32_t m = BloomFilterApp::Utils::OptimalMCounting(n, fpRate) / Defaults::COLLIDE_REGION_SIZE;
            collisionFree = make_unique<Buckets>(m, 1);
        }

        string getFilterName() const
        {
            return "DeletableBloomFilter";
        }

        string getFilterCode() const
        {
            return "DelBF";
        }

        // Returns the maximum capacity of the filter
        uint32_t Capacity() const
        {
            return maxCapacity;
        }

        uint32_t Size() const
        {
            return buckets->count + collisionFree->count;
        }

        // Returns the number of hash functions
        uint32_t K() const
        {
            return k;
        }

        // Returns the number of items in the filter
        uint32_t Count() const
        {
            return count;
        }

        // Returns the target false-positive rate
        double FPrate() const
        {
            return fpRate;
        }

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t> &data) const
        {
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

            // Check if all hash function indices are set in the bucket array
            for (uint32_t hash : hashes)
            {
                if (buckets->Get(hash) == 0)
                {
                    return false;
                }
            }

            return true;
        }

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        DeletableBloomFilter &Add(const std::vector<uint8_t> &data) 
        {
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

            // Set the K bits in the bucket array
            for (auto hash : hashes)
            {
                if (buckets->Get(hash) == 1)
                {
                    uint32_t colFreeHash = (uint32_t)(hash / Defaults::COLLIDE_REGION_SIZE);
                    collisionFree->Set(colFreeHash, 1);
                }
                buckets->Set(hash, 1);
            }

            this->count++;
            return *this;
        }

        bool Remove(const std::vector<uint8_t> &data)
        {
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

            bool removable = 0;
            for (auto hash : hashes)
            {
                uint32_t colFreeHash = (uint32_t)(hash / Defaults::COLLIDE_REGION_SIZE);
                auto collide_state = collisionFree->Get(colFreeHash);
                if (collide_state == 0)
                {
                    buckets->Set(hash, 0);
                    removable = 1;
                }
            //? if all buckets is not collision-free => non-removable
            
            //: if a region is all empty in case each region is a counter, reset the region collide status
            }
            
            this->count--;
            return removable;
        }
        void updateCollideStatus() {
            auto bucketsRear = buckets->count;
        }

        bool TestAndRemove(const vector<uint8_t>& data) {
            if (Test(data)) return Remove(data);
            else return false;
        }

        ~DeletableBloomFilter() {}
    };
}