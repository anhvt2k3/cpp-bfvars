#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels
{

    class CountingBloomFilter : public StaticFilter
    {
    public:
        CountingBloomFilter()
        {
            cout << "CBF constructor without params" << endl;
        }
        CountingBloomFilter(
            uint32_t n, uint8_t b,
            double fpRate,
            uint32_t countExist = 0)
            : StaticFilter(n, Defaults::CBF_BUCKET_SIZE, fpRate, countExist) // Call the base class constructor directly
        {
            cout << "CBF constructor with params" << endl;
        }

        void Init(
            uint32_t n,
            uint8_t b = Defaults::CBF_BUCKET_SIZE,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            uint32_t countExist = 0,
            string algorithm = Defaults::HASH_ALGORITHM,
            string scheme = Defaults::HASH_SCHEME)
        {
            StaticFilter::Init(n, Defaults::CBF_BUCKET_SIZE, fpRate, countExist, algorithm, scheme);
        }

        string getFilterName() const
        {
            return "CountingBloomFilter";
        }

        string getFilterCode() const
        {
            return "CBF";
        }

        // Returns the maximum capacity of the filter
        uint32_t Capacity() const
        {
            return maxCapacity;
        }

        // Returns the filter capacity
        uint32_t Size() const
        {
            return m;
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
            // Check if all hash function indices are set in the bucket array
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

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
        CountingBloomFilter &Add(const std::vector<uint8_t> &data)
        {
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);
            // Set the K bits in the bucket array
            for (auto hash : hashes)
            {
                buckets->Increment(hash, 1);
            }
            this->count++;
            return *this;
        }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t> &data)
        {
            bool member = true;
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

            // Check if all hash function indices are set in the bucket array and set them if not
            for (uint32_t hash : hashes)
            {
                if (buckets->Get(hash) == 0)
                {
                    member = false;
                }
                buckets->Increment(hash, 1);
            }
            count++;
            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndRemove(const std::vector<uint8_t> &data)
        {
            bool member = true;
            std::vector<uint32_t> indices(k); // Store hash function indices
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

            // Calculate hash function indices and check if all are set in the bucket array
            for (int i = 0; i < hashes.size(); ++i)
            {
                indices[i] = hashes[i];
                if (buckets->Get(indices[i]) == 0)
                {
                    member = false;
                }
            }

            // If the data is probably in the filter, decrement the bucket values at the calculated indices
            if (member)
            {
                for (auto index : indices)
                {
                    buckets->Increment(index, -1);
                }
                count--;
            }

            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool Remove(const std::vector<uint8_t> &data)
        {
            vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);

            // Calculate hash function indices and check if all are set in the bucket array
            for (uint32_t hash : hashes)
            {
                buckets->Increment(hash, -1);
            }
            count--;
            return true;
        }

        ~CountingBloomFilter() {}
    };

}