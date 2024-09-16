#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels {

    class StandardCountingBloomFilter : public StaticFilter {
public:
        StandardCountingBloomFilter() {}
        StandardCountingBloomFilter(uint32_t n, uint8_t b, double fpRate, uint32_t countExist = 0) 
            : StaticFilter(n, Defaults::CBF_BUCKET_SIZE, fpRate, countExist)  // Call the base class constructor directly
        {}

        void Init(uint32_t n, uint8_t b = Defaults::CBF_BUCKET_SIZE, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t countExist = 0) override {
            StaticFilter::Init(n, Defaults::CBF_BUCKET_SIZE, fpRate, countExist);
        }

        string getFilterName() const {
            return "StandardCountingBloomFilter";
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
        StandardCountingBloomFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                // cout << "cbf-Adding: " << uint32_t((lower + upper * i) % m) << endl;
                buckets->Increment(uint32_t((lower + upper * i) % m), 1);
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
                buckets->Increment(index, 1);
            }

            count++;
            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndRemove(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            bool member = true;
            std::vector<uint32_t> indices(k); // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                indices[i] = uint32_t((lower + upper * i) % m);
                if (buckets->Get(indices[i]) == 0) {
                    member = false;
                }
            }

            // If the data is probably in the filter, decrement the bucket values at the calculated indices
            if (member) {
                for (auto index : indices) {
                    buckets->Increment(index, -1);
                }
                count--;
            }

            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool Remove(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            uint32_t index; // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                index = uint32_t((lower + upper * i) % m);
                buckets->Increment(index, -1);
            }
            count--;
            return true;
        }

        ~StandardCountingBloomFilter() {}
    };


    class CountingBloomFilter : public StaticFilter {
        // std::unique_ptr<HashAlgorithm> hash; // Hash algorithm object
public:
        CountingBloomFilter() {}
        CountingBloomFilter(uint32_t n, uint8_t b, double fpRate, uint32_t countExist = 0) 
            : StaticFilter(n, Defaults::CBF_BUCKET_SIZE, fpRate, countExist)  // Call the base class constructor directly
        {}

        void Init(uint32_t n, uint8_t b = Defaults::CBF_BUCKET_SIZE, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t countExist = 0)  {
            StaticFilter::Init(n, Defaults::CBF_BUCKET_SIZE, fpRate, countExist);
        }

        string getFilterName() const {
            return "CountingBloomFilter";
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

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data); // Generate hash kernels
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
        CountingBloomFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                // cout << "cbf-Adding: " << uint32_t((lower + upper * i) % m) << endl;
                buckets->Increment(uint32_t((lower + upper * i) % m), 1);
            }

            this->count++;
            return *this;
        }

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        CountingBloomFilter& Reset() {
            buckets->Reset(); // Clear bucket array
            count = 0; // Reset count
            return *this;
        }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            bool member = true;

            // Check if all hash function indices are set in the bucket array and set them if not
            for (uint32_t i = 0; i < k; ++i) {
                uint32_t index = uint32_t((lower + upper * i) % m);
                if (buckets->Get(index) == 0) {
                    member = false;
                }
                buckets->Increment(index, 1);
            }

            count++;
            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndRemove(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            bool member = true;
            std::vector<uint32_t> indices(k); // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                indices[i] = uint32_t((lower + upper * i) % m);
                if (buckets->Get(indices[i]) == 0) {
                    member = false;
                }
            }

            // If the data is probably in the filter, decrement the bucket values at the calculated indices
            if (member) {
                for (auto index : indices) {
                    buckets->Increment(index, -1);
                }
                count--;
            }

            return member;
        }

        // Returns true if the data was probably in the filter, false otherwise.
        bool Remove(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            uint32_t index; // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                index = uint32_t((lower + upper * i) % m);
                buckets->Increment(index, -1);
            }
            count--;
            return true;
        }

        ~CountingBloomFilter() {
        }
    }; // end of CountingBloomFilter

}