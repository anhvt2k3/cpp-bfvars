#include <vector>
#include <list>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <string>

#include "Buckets.h"
#include "../utils/Defaults.h"
#include "../utils/Utils.h"

using namespace std;
using namespace BloomFilterApp;
namespace BloomFilterModels {

    // CountingBloomFilter structure and methods
    class CountingBloomFilter {
        Buckets buckets; // Bucket array
        uint32_t m; // Filter size (number of buckets)
        uint32_t k; // Number of hash functions
        uint32_t count; // Number of items added
        uint32_t maxCapacity; // Maximum capacity of the filter
        double fpRate; // Target false-positive rate
        // std::unique_ptr<HashAlgorithm> hash; // Hash algorithm object
    public:
        CountingBloomFilter() {}
        CountingBloomFilter(uint32_t n, 
                            uint8_t b, 
                            double fpRate,
                            uint32_t countExist                 = 0) :
            buckets    (Buckets(Utils::OptimalMCounting(n, fpRate), b)),   // Initialize buckets 
            m          (Utils::OptimalMCounting(n, fpRate)),                                         // Calculate filter size
            k          (Utils::OptimalKCounting(fpRate)),                                            // Calculate number of hash functions
            count      (countExist),                                                                 // Initialize count
            maxCapacity(n),                                                                          // Set maximum capacity
            fpRate     (fpRate)                                                                      // Set false-positive rate
        {
        }

        // Returns the maximum capacity of the filter
        uint32_t Max_capacity() const {
            return maxCapacity;
        }

        // Returns the filter capacity
        uint32_t Capacity() const {
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

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            auto hashKernel = Utils::HashKernel(data); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Check if all hash function indices are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                if (buckets.Get(uint32_t((lower + upper * i) % m)) == 0)
                {
                    return false;
                }
            }

            return true;
        }

        // Adds the data to the filter.
        // Returns a reference to the filter for chaining.
        CountingBloomFilter& Add(const std::vector<uint8_t>& data) {
            auto hashKernel = Utils::HashKernel(data); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;

            // Set the K bits in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                cout << "cbf-Adding: " << uint32_t((lower + upper * i) % m) << endl;
                buckets.Increment(uint32_t((lower + upper * i) % m), 1);
            }

            this->count++;
            return *this;
        }

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        CountingBloomFilter& Reset() {
            buckets.Reset(); // Clear bucket array
            count = 0; // Reset count
            return *this;
        }

        //@ unsupported
        // Sets the hashing function used in the filter.
        // void SetHash(HashAlgorithm* h) {
        //     hash.reset(h); // Set the hash algorithm object
        // }

        // Tests for membership of the data and adds it to the filter if it doesn't exist.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndAdd(const std::vector<uint8_t>& data) {
            auto hashKernel = Utils::HashKernel(data); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            bool member = true;

            // Check if all hash function indices are set in the bucket array and set them if not
            for (uint32_t i = 0; i < k; ++i) {
                uint32_t index = uint32_t((lower + upper * i) % m);
                if (buckets.Get(index) == 0) {
                    member = false;
                }
                buckets.Increment(index, 1);
            }

            count++;
            return member;
        }

        // Tests for membership of the data and removes it from the filter if it exists.
        // Returns true if the data was probably in the filter, false otherwise.
        bool TestAndRemove(const std::vector<uint8_t>& data) {
            auto hashKernel = Utils::HashKernel(data); // Generate hash kernels
            uint32_t lower = hashKernel.LowerBaseHash;
            uint32_t upper = hashKernel.UpperBaseHash;
            bool member = true;
            std::vector<uint32_t> indices(k); // Store hash function indices

            // Calculate hash function indices and check if all are set in the bucket array
            for (uint32_t i = 0; i < k; ++i) {
                indices[i] = uint32_t((lower + upper * i) % m);
                if (buckets.Get(indices[i]) == 0) {
                    member = false;
                }
            }

            // If the data is probably in the filter, decrement the bucket values at the calculated indices
            if (member) {
                for (auto index : indices) {
                    buckets.Increment(index, -1);
                }
                count--;
            }

            return member;
        }

        ~CountingBloomFilter() {}
    }; // end of CountingBloomFilter

    // CountingScalableBloomFilter structure and methods
    class CountingScalableBloomFilter {
        std::vector<CountingBloomFilter> filters; // List of CountingBloomFilter objects
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each CountingBloomFilter
        uint32_t s; // Scalable growth factor
        // std::time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        void AddFilter(const std::vector<std::vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * std::pow(r, filters.size());
            uint32_t capacity = p * std::pow(s, filters.size());
            CountingBloomFilter newFilter(capacity, 4, fpRate); // Create a new CountingBloomFilter

            // Set the hash algorithm for the new filter if it's not the first filter
            // if (!filters.empty()) {
            //     newFilter.SetHash(filters.front().hash.get()); 
            // }

            // Add data to the new filter if provided
            // if (!data.empty()) {
            //     for (const auto& item : data) {
            //         newFilter.Add(item);
            //     }
            // }

            filters.push_back(newFilter); // Add the new filter to the list
        }
    public:
        CountingScalableBloomFilter(uint32_t p      = Defaults::MAX_COUNT_NUMBER,
                                    double fpRate   = Defaults::FALSE_POSITIVE_RATE,
                                    double r        = Defaults::FILL_RATIO,
                                    uint32_t s      = Defaults::SCALABLE_GROWTH,
                                    const std::vector<std::vector<uint8_t>>& data = {}) :
            r(r), fp(fpRate), p(p), s(s)
        {
            AddFilter(data);
        }

        std::string getConfigure() {
            std::string res = "CSBF Scope" + '\n';
            res += "Tightening-ratio: " + std::__cxx11::to_string(r) + "\n";
            res += "False positive rate: " + std::__cxx11::to_string(fp) + "\n";
            res += "Current max capacity: " + std::__cxx11::to_string(p) + "\n";
            res += "Current filter capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
            res += "Scale growth: " + std::__cxx11::to_string(s) + "\n";
            res += "CBF Scope" + '\n';
            res += "Number of filters: " + std::__cxx11::to_string(filters.size()) + "\n";
            for (int i=0; i<filters.size(); i++) {
                auto filter = filters[i];
                res += "_ _ _ Filter " + std::__cxx11::to_string(i) + " _ _ _\n";
                res += "Filter max capacity: " + std::__cxx11::to_string(filter.Max_capacity()) + "\n";
                res += "Filter capacity: " + std::__cxx11::to_string(filter.Capacity()) + "\n";
                res += "Filter count: " + std::__cxx11::to_string(filter.Count()) + "\n";
                res += "Filter K: " + std::__cxx11::to_string(filter.K()) + "\n";
            }
            return res;
        }


        //@ unsupported
        // CountingScalableBloomFilter(const CreateBloomFilterModel& model) :
        //     r(model.R), fp(model.FP), p(model.P), syncDate(model.SynceDate)
        // {
        //     AddFilter(model.FilterModels);
        // }

        // Returns the current filter capacity.
        uint32_t Capacity() const {
            uint32_t capacity = 0;
            for (const auto& filter : filters) {
                capacity += filter.Capacity();
            }
            return capacity;
        }

        // Returns the number of hash functions used in each filter.
        uint32_t K() const {
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
            // try {
            // }
            // catch (const std::exception& e) {
            //     std::cerr << e.what() << '\n';
            // }
            if (std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter.Count() == filter.Max_capacity(); })) {
                AddFilter(); // Add a new filter if all filters are full
            }
            cout << "csbf-Adding: " << data.data() << endl;
            cout << filters.back().Capacity() << endl;
            cout << filters.back().Max_capacity() << endl;

            filters.back().Add(data); // Add data to the last filter
            cout << "csbf-Added: " << data.data() << endl;
            return *this;
        }

        
        // Removes the data from the filter.
        // Returns a reference to the filter for chaining.
        // CountingScalableBloomFilter& Remove(const std::vector<uint8_t>& data) {
        //     if (std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter.Count() == filter.Max_capacity(); })) {
        //         AddFilter(); // Add a new filter if all filters are full
        //     }
        //     filters.back().Add(data); // Add data to the last filter
        //     return *this;
        // }

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

        //@ unsupported
        // Sets the hashing function used in the filter.
        // void SetHash(HashAlgorithm* h) {
        //     for (auto& filter : filters) {
        //         filter.SetHash(h);
        //     }
        // }

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        CountingScalableBloomFilter& Reset() {
            filters.clear(); // Clear the filter list
            AddFilter(); // Add a new filter
            return *this;
        }

    };

    //@ unsupported
    // Creates a new Scalable Bloom Filter with the specified target false-positive rate and an optimal tightening ratio.
    // Returns a pointer to the created filter.
    // CountingScalableBloomFilter* NewDefaultScalableBloomFilter(double fpRate) {
    //     return new CountingScalableBloomFilter(fpRate, 0.8, 10000); // Create a filter with default parameters
    // }

} // namespace BloomFilterModels