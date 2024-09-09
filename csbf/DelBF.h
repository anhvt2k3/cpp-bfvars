#pragma once
#include "BaseBF.h"
#include "../utils/Utils.h"

using namespace std;
namespace BloomFilterModels {

    class DeletableBloomFilter : public StaticFilter {
        unique_ptr<Buckets> collisionFree; // Bucket array
public:
        DeletableBloomFilter() {}
        DeletableBloomFilter(uint32_t n, 
                            uint8_t b, 
                            double fpRate, 
                            uint32_t countExist = 0) 
            : StaticFilter(n, b, fpRate, countExist)  // Call the base class constructor directly
        {
        }

        void Init(uint32_t n, uint8_t b = 1, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t countExist = 0) override {
            StaticFilter::Init(n, 1, fpRate, countExist);
            collisionFree = make_unique<Buckets>(uint32_t(BloomFilterApp::Utils::OptimalMCounting(n, fpRate)/Defaults::COLLIDE_REGION_SIZE), 1);
        }

        string getFilterName() const {
            return "DeletableBloomFilter";
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
                if (buckets->Get(uint32_t((lower + upper * i) % m)) == 0) {
                    return false;
                }
            }

            return true;
        }

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        DeletableBloomFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                // cout << "cbf-Adding: " << uint32_t((lower + upper * i) % m) << endl;
                uint32_t bucketIndex = uint32_t((lower + upper * i) % m);
                if (buckets->Get(bucketIndex)) {
                    collisionFree->Set(uint32_t(bucketIndex/Defaults::COLLIDE_REGION_SIZE), 1);
                }
                buckets->Set(bucketIndex, 1);
            }

            this->count++;
            return *this;
        }

        bool Remove(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            bool removable = 0;
            for (uint32_t i = 0; i < k; ++i) {
                uint32_t bucketIndex = uint32_t((lower + upper * i) % m);
                if (collisionFree->Get(uint32_t(bucketIndex/Defaults::COLLIDE_REGION_SIZE)) == 0) {
                    buckets->Set(bucketIndex, 0);
                    removable = 1;
                }
            //@ if all buckets is not collision-free => non-removable
            }
            return removable;
        }

        bool TestAndRemove(const vector<uint8_t>& data) {
            if (Test(data)) return Remove(data);
            else return false;
        }

        ~DeletableBloomFilter() {}
    };
}