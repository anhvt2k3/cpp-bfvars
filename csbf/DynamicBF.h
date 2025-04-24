#pragma once
#include "../utils/Utils.h"
#include "CountingBF.h"


using namespace BloomFilterApp;
namespace BloomFilterModels {
    
    class DynamicBloomFilter : public DynamicFilter {
        // vector<shared_ptr<CountingBloomFilter>> filters;
        double fp; // Target false-positive rate
        uint32_t c; // Maximum item count for each CountingBloomFilter
        // time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            filters.push_back(make_shared<CountingBloomFilter>(c, 4, fp));
            return 0;
        }
public:
        DynamicBloomFilter(uint32_t c = Defaults::MAX_COUNT_NUMBER, double fp = Defaults::FALSE_POSITIVE_RATE) : c(c), fp(fp) {
            AddFilter(); // Add the first filter
        }

        string getFilterName() const {
            return "DynamicBloomFilter";
        }

        string getFilterCode() const {
            return "DynamicBF";
        }

        std::string getConfigure() {
            std::string res = "_ _ _ CSBF Scope _ _ _ \n";
            res += "False positive rate: " + std::__cxx11::to_string(FPrate()) + "\n";
            res += "Current capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
            res += "Current filter size: " + std::__cxx11::to_string(Size()) + "\n";
            res += "CBF Scope \n";
            res += "Number of filters: " + std::__cxx11::to_string(filters.size()) + "\n";
            
            for (int i=0; i<filters.size(); i++) {
                auto filter = filters[i];
                res += "_ _ _ Filter " + std::__cxx11::to_string(i) + " _ _ _\n";
                res += filter->getConfigure();
            }
            return res;
        }

        // Returns the maximum capacity of the filter->
        uint32_t Capacity() const {
            uint32_t c = 0;
            for (const auto& filter : filters) {
                c += filter->Capacity();
            }
            return c;
        }

        // Return False Positive Rate
        double FPrate() const {
            return fp;
        }

        // Returns the current filter size.
        uint32_t Size() const {
            uint32_t size = 0;
            for (const auto& filter : filters) {
                size += filter->Size();
            }
            return size;
        }

        // Returns the number of hash functions used in each filter->
        uint32_t K() const {
            return filters.empty() ? 0 : filters.front()->K(); // Return the K of the first filter
        }

        uint32_t Count() const {
            uint32_t count = 0;
            for (const auto& filter : filters) {
                count += filter->Count();
            }
            return count;
        }

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            // Check for membership in each filter
            for (const auto& filter : filters) {
                if (filter->Test(data)) {
                    return true;
                }
            }
            return false;
        }

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        DynamicBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (filters.empty() || std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
                AddFilter(); // Add a new filter if all filters are full
            }

            filters.back()->Add(data); // Add data to the last filter
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
            int isNotUnique = -1; //# -1: value not found, 0: value found once, >0: value found multiple times
            int i = 0;
            int filter_pos = 0;
            for (auto& filter : filters) {
                if (filter->Test(data)) {
                    isNotUnique++;
                    filter_pos = i;
                }
                i++;
            }
            if (isNotUnique == 0) {
                filters[filter_pos]->TestAndRemove(data);
                return true;
            }
            return false;
        }
        
    DynamicBloomFilter& Reset() {
        filters.clear(); // Clear the filter list
        // AddFilter(); // Add a new filter
        return *this;
    }
    };
    
    class DynamicStdCountingBloomFilter : public DynamicFilter {
        // vector<shared_ptr<CountingBloomFilter>> filters;
        double fp; // Target false-positive rate
        uint32_t c; // Maximum item count for each CountingBloomFilter
        // time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            filters.push_back(make_shared<CountingBloomFilter>(c, 4, fp));
            return 0;
        }
public:
        DynamicStdCountingBloomFilter(uint32_t c = Defaults::MAX_COUNT_NUMBER, double fp = Defaults::FALSE_POSITIVE_RATE) : c(c), fp(fp) {
            AddFilter(); // Add the first filter
        }

        string getFilterName() const {
            return "DynamicStdCountingBloomFilter";
        }

        string getFilterCode() const {
            return "DynamicCBF";
        }

        std::string getConfigure() {
            std::string res = "_ _ _ CSBF Scope _ _ _ \n";
            res += "False positive rate: " + std::__cxx11::to_string(FPrate()) + "\n";
            res += "Current capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
            res += "Current filter size: " + std::__cxx11::to_string(Size()) + "\n";
            res += "CBF Scope \n";
            res += "Number of filters: " + std::__cxx11::to_string(filters.size()) + "\n";
            for (int i=0; i<filters.size(); i++) {
                auto filter = filters[i];
                res += "_ _ _ Filter " + std::__cxx11::to_string(i) + " _ _ _\n";
                res += filter->getConfigure();
            }
            return res;
        }

        // Returns the maximum capacity of the filter->
        uint32_t Capacity() const {
            uint32_t c = 0;
            for (const auto& filter : filters) {
                c += filter->Capacity();
            }
            return c;
        }

        // Return False Positive Rate
        double FPrate() const {
            return fp;
        }

        // Returns the current filter size.
        uint32_t Size() const {
            uint32_t size = 0;
            for (const auto& filter : filters) {
                size += filter->Size();
            }
            return size;
        }

        // Returns the number of hash functions used in each filter->
        uint32_t K() const {
            return filters.empty() ? 0 : filters.front()->K(); // Return the K of the first filter
        }

        uint32_t Count() const {
            uint32_t count = 0;
            for (const auto& filter : filters) {
                count += filter->Count();
            }
            return count;
        }

        // Tests for membership of the data.
        // Returns true if the data is probably a member, false otherwise.
        bool Test(const std::vector<uint8_t>& data) const {
            // Check for membership in each filter
            for (const auto& filter : filters) {
                if (filter->Test(data)) {
                    return true;
                }
            }
            return false;
        }

        // Adds the data to the filter->
        // Returns a reference to the filter for chaining.
        DynamicStdCountingBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (filters.empty() || std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
                AddFilter(); // Add a new filter if all filters are full
            }

            filters.back()->Add(data); // Add data to the last filter
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
            int isNotUnique = -1; //# -1: value not found, 0: value found once, >0: value found multiple times
            int i = 0;
            int filter_pos = 0;
            for (auto& filter : filters) {
                if (filter->Test(data)) {
                    isNotUnique++;
                    filter_pos = i;
                }
                i++;
            }
            if (isNotUnique == 0) {
                filters[filter_pos]->TestAndRemove(data);
                return true;
            }
            return false;
        }


    DynamicStdCountingBloomFilter& Reset() {
        filters.clear(); // Clear the filter list
        // AddFilter(); // Add a new filter
        return *this;
    }
    };
}