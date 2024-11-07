#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"

using namespace std;

namespace BloomFilterModels
{

    class StandardBloomFilter : public StaticFilter
    {
    public:
        StandardBloomFilter() {}
        StandardBloomFilter(
            uint32_t n,
            uint8_t b,
            double fpRate,
            string algorithm,
            uint8_t bitRange,
            uint32_t countExist = 0)
            : StaticFilter(n, b, fpRate, algorithm, bitRange, countExist) // Call the base class constructor directly
        {
        }

        void Init(
            uint32_t n,
            uint8_t b = Defaults::CBF_BUCKET_SIZE,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            string algorithm = Defaults::HASH_ALGORITHM,
            uint8_t bitRange = Defaults::HASH_BIT_RANGE,
            uint32_t countExist = 0)
        {
            StaticFilter::Init(n, b, fpRate, algorithm, bitRange, countExist);
        }

        string getFilterName() const
        {
            return "StandardBloomFilter";
        }

        string getFilterCode() const
        {
            return "StdBF";
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
            if (this->hashBitRange == 64)
            {
                return this->test<HashPair64, uint32_t>(data);
            }
            else if (this->hashBitRange == 128)
            {
                return this->test<HashPair128, uint64_t>(data);
            }
        }

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        StandardBloomFilter &Add(const std::vector<uint8_t> &data)
        {
            if (this->hashBitRange == 64)
            {
                return this->add<HashPair64, uint32_t>(data);
            }
            else if (this->hashBitRange == 128)
            {
                return this->add<HashPair128, uint64_t>(data);
            }
            assert(1 == 0);
        }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t> &data)
        {
            if (this->hashBitRange == 64)
            {
                return this->testAndAdd<HashPair64, uint32_t>(data);
            }
            else if (this->hashBitRange == 128)
            {
                return this->testAndAdd<HashPair128, uint64_t>(data);
            }
        }

        ~StandardBloomFilter() {}

    private:
        template <typename HashPairType, typename EvalType>
        bool test(const std::vector<uint8_t> &data) const
        {
            HashPairType hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(
                data,
                this->hashAlgorithm);
            EvalType lower = hashKernel.LowerBaseHash;
            EvalType upper = hashKernel.UpperBaseHash;

            // Check if all hash function indices are set in the bucket array
            for (EvalType i = 0; i < k; ++i)
            {
                if (buckets->Get(EvalType((lower + upper * i) % m)) == 0)
                {
                    return false;
                }
            }
            return true;
        }

        template <typename HashPairType, typename EvalType>
        StandardBloomFilter &add(const std::vector<uint8_t> &data)
        {
            HashPairType hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(data, this->hashAlgorithm); // Generate hash kernels
            EvalType lower = hashKernel.LowerBaseHash;
            EvalType upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (EvalType i = 0; i < k; ++i)
            {
                // cout << "cbf-Adding: " << EvalType((lower + upper * i) % m) << endl;
                buckets->Set(EvalType((lower + upper * i) % m), 1);
            }

            this->count++;
            return *this;
        }

        template <typename HashPairType, typename EvalType>
        bool testAndAdd(const std::vector<uint8_t> &data)
        {
            HashPairType hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(data, this->hashAlgorithm); // Generate hash kernels
            EvalType lower = hashKernel.LowerBaseHash;
            EvalType upper = hashKernel.UpperBaseHash;
            bool member = true;

            // Check if all hash function indices are set in the bucket array and set them if not
            for (EvalType i = 0; i < k; ++i)
            {
                EvalType index = EvalType((lower + upper * i) % m);
                if (buckets->Get(index) == 0)
                {
                    member = false;
                }
                buckets->Set(index, 1);
            }

            count++;
            return member;
        }
    };

}