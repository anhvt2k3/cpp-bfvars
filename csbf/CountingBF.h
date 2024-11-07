#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels
{

    class CountingBloomFilter : public StaticFilter
    {
    public:
        CountingBloomFilter() {}
        CountingBloomFilter(
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
            uint32_t countExist = 0) override
        {
            StaticFilter::Init(n, b, fpRate, algorithm, bitRange, countExist);
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
        CountingBloomFilter &Add(const std::vector<uint8_t> &data)
        {
            if (this->hashBitRange == 64)
            {
                return this->add<HashPair64, uint32_t>(data);
            }
            else if (this->hashBitRange == 128)
            {
                return this->add<HashPair128, uint64_t>(data);
            }
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

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndRemove(const std::vector<uint8_t> &data)
        {
            if (this->hashBitRange == 64)
            {
                return this->testAndRemove<HashPair64, uint32_t>(data);
            }
            else if (this->hashBitRange == 128)
            {
                return this->testAndRemove<HashPair128, uint64_t>(data);
            }
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool Remove(const std::vector<uint8_t> &data)
        {
            if (this->hashBitRange == 64)
            {
                return this->remove<HashPair64, uint32_t>(data);
            }
            else if (this->hashBitRange == 128)
            {
                return this->remove<HashPair128, uint64_t>(data);
            }
        }
        ~CountingBloomFilter() {}

    private:
        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        template <typename HashPairType, typename EvalType>
        bool test(const std::vector<uint8_t> &data) const
        {
            auto hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(data, this->hashAlgorithm); // Generate hash kernels
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

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        template <typename HashPairType, typename EvalType>
        CountingBloomFilter &add(const std::vector<uint8_t> &data)
        {
            auto hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(data, this->hashAlgorithm); // Generate hash kernels
            EvalType lower = hashKernel.LowerBaseHash;
            EvalType upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (EvalType i = 0; i < k; ++i)
            {
                // cout << "cbf-Adding: " << EvalType((lower + upper * i) % m) << endl;
                buckets->Increment(EvalType((lower + upper * i) % m), 1);
            }

            this->count++;
            return *this;
        }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        template <typename HashPairType, typename EvalType>
        bool testAndAdd(const std::vector<uint8_t> &data)
        {
            auto hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(data, this->hashAlgorithm); // Generate hash kernels
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
                buckets->Increment(index, 1);
            }

            count++;
            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        template <typename HashPairType, typename EvalType>
        bool testAndRemove(const std::vector<uint8_t> &data)
        {
            auto hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(data, this->hashAlgorithm); // Generate hash kernels
            EvalType lower = hashKernel.LowerBaseHash;
            EvalType upper = hashKernel.UpperBaseHash;
            bool member = true;
            std::vector<EvalType> indices(k); // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (EvalType i = 0; i < k; ++i)
            {
                indices[i] = EvalType((lower + upper * i) % m);
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
        template <typename HashPairType, typename EvalType>
        bool remove(const std::vector<uint8_t> &data)
        {
            auto hashKernel = BloomFilterApp::Utils::GenerateHashKernel<HashPairType>(data, this->hashAlgorithm); // Generate hash kernels
            EvalType lower = hashKernel.LowerBaseHash;
            EvalType upper = hashKernel.UpperBaseHash;
            EvalType index; // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (EvalType i = 0; i < k; ++i)
            {
                index = EvalType((lower + upper * i) % m);
                buckets->Increment(index, -1);
            }
            count--;
            return true;
        }
    };

    class CryptoCountingBloomFilter : public CountingBloomFilter
    {
        // std::unique_ptr<HashAlgorithm> hash; // Hash algorithm object
    public:
        CryptoCountingBloomFilter() {}
        CryptoCountingBloomFilter(
            uint32_t n,
            uint8_t b,
            double fpRate,
            string algorithm,
            uint8_t bitRange,
            uint32_t countExist = 0)
            : CountingBloomFilter(n, b, fpRate, algorithm, bitRange, countExist) // Call the base class constructor directly
        {
        }

        void Init(
            uint32_t n,
            uint8_t b = Defaults::CBF_BUCKET_SIZE,
            double fpRate = Defaults::FALSE_POSITIVE_RATE,
            string algorithm = Defaults::CRYPTO_HASH_ALGORITHM,
            uint8_t bitRange = Defaults::HASH_BIT_RANGE,
            uint32_t countExist = 0) override
        {
            CountingBloomFilter::Init(n, b, fpRate, algorithm, bitRange, countExist);
        }

        string getFilterName() const
        {
            return "CryptoCountingBloomFilter";
        }

        string getFilterCode() const
        {
            return "CryptoCBF";
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

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        CryptoCountingBloomFilter &Reset()
        {
            buckets->Reset(); // Clear bucket array
            count = 0;        // Reset count
            return *this;
        }

        ~CryptoCountingBloomFilter()
        {
        }
    }; // end of CryptoCountingBloomFilter

}