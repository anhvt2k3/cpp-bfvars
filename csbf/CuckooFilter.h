#include "BaseBF.h"
#include "./CountingBF.h"

namespace BloomFilterModels {
    class CuckooFilter : public CuckooBase {
public:
        CuckooFilter() {}
        // L is a number of 2^i for i>=2
        CuckooFilter(uint32_t n, uint8_t b, double fpRate, uint32_t k = 0, uint32_t countExist = 0) 
            : CuckooBase(n, Defaults::CBF_BUCKET_SIZE, fpRate, k, countExist)  // Call the base class constructor directly
        {
        }

        // L is a number of 2^i for i>=2
        void Init(uint32_t n, uint8_t b = Defaults::CBF_BUCKET_SIZE, double fpRate = Defaults::FALSE_POSITIVE_RATE, uint32_t k = 0, uint32_t countExist = 0) override {
            CuckooBase::Init(n, Defaults::CBF_BUCKET_SIZE, fpRate, k, countExist);
        }

        string getFilterName() const {
            return "CuckooFilter";
        }

        string getFilterCode() const {
            return "VIBF";
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

        uint32_t fingerprint(const vector<uint8_t>& data) const {
            uint32_t result = 0;
            for (auto byte : data) {
                result ^= byte;
            }
            return result;
        }

        uint32_t hash_(vector<uint8_t>& key) const {
            uint32_t hashvalue;
            hashvalue = BloomFilterApp::Utils::HashKernel(key, "single").LowerBaseHash;
            return hashvalue;
        }

        uint32_t hash_(uint32_t key) const {
            uint32_t hashvalue;
            hashvalue = key;
            return hashvalue;
        }

        //: Bucket structure = <1-bit of isOccupied> + <fingerprint>
        //* Entries index is bound within cargosize

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Check if all hash function indices are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                uint32_t hi = uint32_t((lower + upper * i) % m);
                uint32_t gi = uint32_t((upper + lower * i) % (dL->count) );
                uint32_t vgi = dL->Get(gi);
                uint32_t c = buckets->Get(hi);
                if (c - vgi != 0 && c - vgi < dL->Get(0))
                {
                    return false;
                }
            }

            return true;
        }

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        CuckooFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            for (uint32_t i = 0; i < k; ++i) {
                uint32_t hi = uint32_t((lower + upper * i) % m);
                uint32_t gi = uint32_t((upper + lower * i) % (dL->count) );
                uint32_t vgi = dL->Get(gi);
                buckets->Increment(hi, vgi);
            }

            this->count++;
            return *this;
        }

        CuckooFilter& Remove(const std::vector<uint8_t>& data) {
            auto hashKernel = BloomFilterApp::Utils::HashKernel(data, "murmur"); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            for (uint32_t i = 0; i < k; ++i) {
                uint32_t hi = uint32_t((lower + upper * i) % m);
                uint32_t gi = uint32_t((upper + lower * i) % (dL->count) );
                uint32_t vgi = dL->Get(gi);
                buckets->Increment(hi, -vgi);
            }

            this->count--;
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
                uint32_t hi = uint32_t((lower + upper * i) % m);
                uint32_t gi = uint32_t((upper + lower * i) % (dL->count) );
                uint32_t vgi = dL->Get(gi);
                if (buckets->Get(hi) == 0) {
                    member = false;
                }
                buckets->Set(hi, vgi);
            }

            count++;
            return member;
        }

        ~CuckooFilter() {}

    
};

};