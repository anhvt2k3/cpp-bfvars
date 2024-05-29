#include <vector>
#include <list>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ctime>

#include "Utils.h"

using namespace BloomFilterApp;
namespace BloomFilterModels {

    // CountingBloomFilter structure and methods
    struct CountingBloomFilter {
        std::vector<uint8_t> buckets; // Bucket array for data storage
        std::size_t m; // Filter size (number of buckets)
        std::size_t k; // Number of hash functions
        uint32_t count; // Number of items added
        uint32_t maxCapacity; // Maximum capacity of the filter
        double fpRate; // Target false-positive rate
        // std::unique_ptr<HashAlgorithm> hash; // Hash algorithm object

        CountingBloomFilter(std::size_t n, uint8_t b, double fpRate, const std::vector<uint8_t>& data = {}, uint32_t countExist = 0) :
            buckets(Utils::OptimalMCounting(n, fpRate), b), // Initialize buckets with specified size and bit width
            m(Utils::OptimalMCounting(n, fpRate)), // Calculate filter size
            k(Utils::OptimalKCounting(fpRate)), // Calculate number of hash functions
            count(countExist), // Initialize count
            maxCapacity(n), // Set maximum capacity
            fpRate(fpRate), // Set false-positive rate
            hash(Defaults::GetDefaultHashAlgorithm()) // Create a hash algorithm object
        {
            // If data is provided, add it to the filter
            if (!data.empty()) {
                for (const auto& item : data) {
                    Add(item);
                }
            }
        }

        // Returns the filter capacity
        std::size_t Capacity() const {
            return m;
        }

        // Returns the number of hash functions
        std::size_t K() const {
            return k;
        }

        // Returns the number of items in the filter
        uint32_t Count() const {
            return count;
        }

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            auto hashKernel = Utils::HashKernel(data, hash.get()); // Generate hash kernels
            std::size_t lower = hashKernel.LowerBaseHash;
            std::size_t upper = hashKernel.UpperBaseHash;

            // Check if all hash function indices are set in the bucket array
            for (std::size_t i = 0; i < k; ++i) {
                std::size_t index = (lower + upper * i) % m;
                if (buckets[index] == 0) {
                    return false;
                }
            }

            return true;
        }

        // Adds the data to the filter.
        // Returns a reference to the filter for chaining.
        CountingBloomFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = Utils::HashKernel(data, hash.get()); // Generate hash kernels
            std::size_t lower = hashKernel.LowerBaseHash;
            std::size_t upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (std::size_t i = 0; i < k; ++i) {
                std::size_t index = (lower + upper * i) % m;
                buckets[index]++;
            }

            count++;
            return *this;
        }

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        CountingBloomFilter& Reset() {
            std::fill(buckets.begin(), buckets.end(), 0); // Clear bucket array
            count = 0; // Reset count
            return *this;
        }

        // Sets the hashing function used in the filter.
        void SetHash(HashAlgorithm* h) {
            hash.reset(h); // Set the hash algorithm object
        }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t>& data) {
            auto hashKernel = Utils::HashKernel(data, hash.get()); // Generate hash kernels
            std::size_t lower = hashKernel.LowerBaseHash;
            std::size_t upper = hashKernel.UpperBaseHash;
            bool member = true;

            // Check if all hash function indices are set in the bucket array and set them if not
            for (std::size_t i = 0; i < k; ++i) {
                std::size_t index = (lower + upper * i) % m;
                if (buckets[index] == 0) {
                    member = false;
                }
                buckets[index]++;
            }

            count++;
            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndRemove(const std::vector<uint8_t>& data) {
            auto hashKernel = Utils::HashKernel(data, hash.get()); // Generate hash kernels
            std::size_t lower = hashKernel.LowerBaseHash;
            std::size_t upper = hashKernel.UpperBaseHash;
            bool member = true;
            std::vector<std::size_t> indices(k); // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (std::size_t i = 0; i < k; ++i) {
                indices[i] = (lower + upper * i) % m;
                if (buckets[indices[i]] == 0) {
                    member = false;
                }
            }

            // If the data is probably in the filter, decrement the bucket values at the calculated indices
            if (member) {
                for (auto index : indices) {
                    buckets[index]--;
                }
                count--;
            }

            return member;
        }
    };

    // CountingScalableBloomFilter structure and methods
    struct CountingScalableBloomFilter {
        std::list<CountingBloomFilter> filters; // List of CountingBloomFilter objects
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each CountingBloomFilter
        uint32_t s; // Scalable growth factor
        std::time_t syncDate; // Synchronization date

        CountingScalableBloomFilter(double fpRate, double r, uint32_t p = Defaults::MAX_COUNT_NUMBER, uint32_t s = Defaults::SCALABLE_GROWTH, const std::vector<std::vector<uint8_t>>& data = {}) :
            r(r), fp(fpRate), p(p), s(s), syncDate(std::time(nullptr))
        {
            AddFilter(data);
        }

        CountingScalableBloomFilter(const CreateBloomFilterModel& model) :
            r(model.R), fp(model.FP), p(model.P), syncDate(model.SynceDate)
        {
            AddFilter(model.FilterModels);
        }

        // Returns the current filter capacity.
        uint32_t Capacity() const {
            uint32_t capacity = 0;
            for (const auto& filter : filters) {
                capacity += filter.Capacity();
            }
            return capacity;
        }

        // Returns the number of hash functions used in each filter.
        std::size_t K() const {
            return filters.empty() ? 0 : filters.front().K(); // Return the K of the first filter
        }

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            // Check for membership in each filter
            for (const auto& filter : filters) {
                if (filter.Test(data)) {
                    return true;
                }
            }
            return false;
        }

        // Adds the data to the filter.
        // Returns a reference to the filter for chaining.
        CountingScalableBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter.Count() == filter.maxCapacity; })) {
                AddFilter(); // Add a new filter if all filters are full
            }
            filters.back().Add(data); // Add data to the last filter
            return *this;
        }

        // Removes the data from the filter.
        // Returns a reference to the filter for chaining.
        CountingScalableBloomFilter& Remove(const std::vector<uint8_t>& data) {
            if (std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter.Count() == filter.maxCapacity; })) {
                AddFilter(); // Add a new filter if all filters are full
            }
            filters.back().Add(data); // Add data to the last filter
            return *this;
        }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t>& data) {
            bool member = Test(data);
            if (!member) {
                Add(data);
            }
            return !member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndRemove(const std::vector<uint8_t>& data) {
            for (auto& filter : filters) {
                if (filter.Test(data)) {
                    filter.TestAndRemove(data);
                    return true;
                }
            }
            return false;
        }

        // Sets the hashing function used in the filter.
        void SetHash(HashAlgorithm* h) {
            for (auto& filter : filters) {
                filter.SetHash(h);
            }
        }

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        CountingScalableBloomFilter& Reset() {
            filters.clear(); // Clear the filter list
            AddFilter(); // Add a new filter
            return *this;
        }

        // Adds a new filter to the list with restricted false-positive rate.
        void AddFilter(const std::vector<std::vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * std::pow(r, filters.size());
            uint32_t capacity = p * std::pow(s, filters.size());
            CountingBloomFilter newFilter(capacity, 4, fpRate, {}); // Create a new CountingBloomFilter

            // Set the hash algorithm for the new filter if it's not the first filter
            if (!filters.empty()) {
                newFilter.SetHash(filters.front().hash.get()); 
            }

            // Add data to the new filter if provided
            if (!data.empty()) {
                for (const auto& item : data) {
                    newFilter.Add(item);
                }
            }

            filters.push_back(newFilter); // Add the new filter to the list
        }
    };

    // Creates a new Scalable Bloom Filter with the specified target false-positive rate and an optimal tightening ratio.
    // Returns a pointer to the created filter.
    CountingScalableBloomFilter* NewDefaultScalableBloomFilter(double fpRate) {
        return new CountingScalableBloomFilter(fpRate, 0.8, 10000); // Create a filter with default parameters
    }

} // namespace BloomFilterModels