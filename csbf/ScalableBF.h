#pragma once
#include "../utils/Utils.h"
#include "CountingBF.h"
#include "DelBF.h"
#include "BF.h"

using namespace std;
namespace BloomFilterModels {

    class ScalableBase {
    public:
        uint32_t TightenedK(
            uint32_t k,
            uint32_t i,
            double r
        ) {
            return k + i*log2(1/r);
        }
    };

class CountingScalableBloomFilter : public DynamicFilter, public ScalableBase {
    // vector<shared_ptr<CountingBloomFilter>> filters;
    double r; // Tightening ratio
    double fp; // Target false-positive rate
    uint32_t p; // Maximum item count for each CountingBloomFilter
    uint32_t s; // Scalable growth factor
    // time_t syncDate; // Synchronization date

    // Adds a new filter to the list with restricted false-positive rate.
    int AddFilter(const vector<vector<uint8_t>>& data = {}) {
        // Calculate false-positive rate and capacity for the new filter
        double fpRate = fp * pow(r, filters.size());
        uint32_t capacity = p * pow(s, filters.size());
        uint32_t k_ = (!filters.empty()) ? TightenedK(filters.front()->K(),filters.size()-1,r) : 0;
        
        filters.push_back(make_shared<CountingBloomFilter>(capacity, 4, fpRate, k_));
        // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
        return 0;
    }
public:
    CountingScalableBloomFilter() : r   (Defaults::TIGHTENING_RATIO),
                                    fp  (Defaults::FALSE_POSITIVE_RATE),
                                    p   (Defaults::MAX_COUNT_NUMBER),
                                    s   (Defaults::SCALABLE_GROWTH)
    {
        // AddFilter(); // Add the first filter
        cout << "CountingScaleBF created without params.\n";
    }
    
    string getFilterName() const {
        return "CountingScalableBloomFilter";
    }

    string getFilterCode() const {
        return "CSBF";
    }

    std::string getConfigure() {
        std::string res = "_ _ _ CSBF Scope _ _ _ \n";
        res += "Tightening-ratio: " + std::__cxx11::to_string(r) + "\n";
        res += "False positive rate: " + std::__cxx11::to_string(fp) + "\n";
        res += "Current max capacity: " + std::__cxx11::to_string(p) + "\n";
        res += "Current filter capacity: " + std::__cxx11::to_string(Size()) + "\n";
        res += "Scale growth: " + std::__cxx11::to_string(s) + "\n";
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
    CountingScalableBloomFilter& Add(const std::vector<uint8_t>& data) {
        if (filters.empty() || std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
            AddFilter(); // Add a new filter if all filters are full
        }
        // cout << "csbf-Adding: " << data.data() << endl;
        // cout << filters.back().Size() << endl;
        // cout << filters.back().Capacity() << endl;

        filters.back()->Add(data); // Add data to the last filter
        // cout << "csbf-Added: " << data.data() << endl;
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

    // Resets the filter to its original state.
    // Returns a reference to the filter for chaining.
    CountingScalableBloomFilter& Reset() {
        filters.clear(); // Clear the filter list
        AddFilter(); // Add a new filter
        return *this;
    }

    ~CountingScalableBloomFilter() {
    }

};
    
    class CryptoCountingScalableBloomFilter : public DynamicFilter, public ScalableBase {
        // vector<shared_ptr<CryptoCountingBloomFilter>> filters;
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each CryptoCountingBloomFilter
        uint32_t s; // Scalable growth factor
        time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * pow(r, filters.size());
            uint32_t capacity = p * pow(s, filters.size());
            uint32_t k_ = (!filters.empty()) ? TightenedK(filters.front()->K(),filters.size()-1,r) : 0;
            
            filters.push_back(make_shared<CryptoCountingBloomFilter>(capacity, 4, fpRate, k_));
            // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
            return 0;
        }
    public:
        CryptoCountingScalableBloomFilter() : r   (Defaults::TIGHTENING_RATIO),
                                        fp  (Defaults::FALSE_POSITIVE_RATE),
                                        p   (Defaults::MAX_COUNT_NUMBER),
                                        s   (Defaults::SCALABLE_GROWTH)
        {
            // AddFilter(); // Add the first filter
        }
        
        string getFilterName() const {
            return "CryptoCountingScalableBloomFilter";
        }

        std::string getConfigure() {
            std::string res = "_ _ _ CSBF Scope _ _ _ \n";
            res += "Tightening-ratio: " + std::__cxx11::to_string(r) + "\n";
            res += "False positive rate: " + std::__cxx11::to_string(FPrate()) + "\n";
            res += "Current capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
            res += "Current filter size: " + std::__cxx11::to_string(Size()) + "\n";
            res += "Scale growth: " + std::__cxx11::to_string(s) + "\n";
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
        CryptoCountingScalableBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (filters.empty() || std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
                AddFilter(); // Add a new filter if all filters are full
            }
            // cout << "csbf-Adding: " << data.data() << endl;
            // cout << filters.back().Size() << endl;
            // cout << filters.back().Capacity() << endl;

            filters.back()->Add(data); // Add data to the last filter
            // cout << "csbf-Added: " << data.data() << endl;
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

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        CryptoCountingScalableBloomFilter& Reset() {
            filters.clear(); // Clear the filter list
            // AddFilter(); // Add a new filter
            return *this;
        }

        ~CryptoCountingScalableBloomFilter() {
        }

};

    class ScalableDeletableBloomFilter : public DynamicFilter, public ScalableBase {
        // vector<shared_ptr<DeletableBloomFilter>> filters;
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each DeletableBloomFilter
        uint32_t s; // Scalable growth factor
        time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * pow(r, filters.size());
            uint32_t capacity = p * pow(s, filters.size());
            uint32_t k = (!filters.empty()) ? TightenedK(filters.front()->K(),filters.size()-1,r) : 0;
            
            filters.push_back(make_shared<DeletableBloomFilter>(capacity, 1, fpRate, k));
            // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
            return 0;
        }
    public:
        ScalableDeletableBloomFilter() : r   (Defaults::TIGHTENING_RATIO),
                                        fp  (Defaults::FALSE_POSITIVE_RATE),
                                        p   (Defaults::MAX_COUNT_NUMBER),
                                        s   (Defaults::SCALABLE_GROWTH)
        {
            // AddFilter(); // Add the first filter
            cout << "ScaleGBF created without params.\n";
        }
        
        string getFilterName() const {
            return "ScalableDeletableBloomFilter";
        }

        string getFilterCode() const {
            return "ScalableDBF";
        }


        std::string getConfigure() {
            std::string res = "_ _ _ CSBF Scope _ _ _ \n";
            res += "Tightening-ratio: " + std::__cxx11::to_string(r) + "\n";
            res += "False positive rate: " + std::__cxx11::to_string(FPrate()) + "\n";
            res += "Current capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
            res += "Current filter size: " + std::__cxx11::to_string(Size()) + "\n";
            res += "Scale growth: " + std::__cxx11::to_string(s) + "\n";
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
        ScalableDeletableBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (filters.empty() || std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
                AddFilter(); // Add a new filter if all filters are full
            }
            // cout << "csbf-Adding: " << data.data() << endl;
            // cout << filters.back().Size() << endl;
            // cout << filters.back().Capacity() << endl;

            filters.back()->Add(data); // Add data to the last filter
            // cout << "csbf-Added: " << data.data() << endl;
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

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        ScalableDeletableBloomFilter& Reset() {
            filters.clear(); // Clear the filter list
            // AddFilter(); // Add a new filter
            return *this;
        }

        ~ScalableDeletableBloomFilter() {
        }
};

    class ScalableStandardBloomFilter : public DynamicFilter, public ScalableBase {
        // vector<shared_ptr<StandardBloomFilter>> filters;
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each StandardBloomFilter
        uint32_t s; // Scalable growth factor
        time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * pow(r, filters.size());
            uint32_t capacity = p * pow(s, filters.size());
            
            uint32_t k_ = 0;
            k_ = (!filters.empty()) ? TightenedK(filters.front()->K(),filters.size()-1,r) : 0;
            filters.push_back(make_shared<StandardBloomFilter>(capacity, 1, fpRate, k_));
            // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
            return 0;
        }
    public:
        ScalableStandardBloomFilter() : r   (Defaults::TIGHTENING_RATIO),
                                        fp  (Defaults::FALSE_POSITIVE_RATE),
                                        p   (Defaults::MAX_COUNT_NUMBER),
                                        s   (Defaults::SCALABLE_GROWTH)
        {
            cout << "ScaleStdBF created without params.\n";
            // AddFilter(); // Add the first filter
        }
        
        string getFilterName() const {
            return "ScalableStandardBloomFilter";
        }

        string getFilterCode() const {
            return "ScalableSBF";
        }


        std::string getConfigure() {
            std::string res = "_ _ _ CSBF Scope _ _ _ \n";
            res += "Tightening-ratio: " + std::__cxx11::to_string(r) + "\n";
            res += "False positive rate: " + std::__cxx11::to_string(FPrate()) + "\n";
            res += "Current capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
            res += "Current filter size: " + std::__cxx11::to_string(Size()) + "\n";
            res += "Scale growth: " + std::__cxx11::to_string(s) + "\n";
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
        ScalableStandardBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (filters.empty() || std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
                AddFilter(); // Add a new filter if all filters are full
            }
            // cout << "csbf-Adding: " << data.data() << endl;
            // cout << filters.back().Size() << endl;
            // cout << filters.back().Capacity() << endl;

            filters.back()->Add(data); // Add data to the last filter
            // cout << "csbf-Added: " << data.data() << endl;
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

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        ScalableStandardBloomFilter& Reset() {
            filters.clear(); // Clear the filter list
            // AddFilter(); // Add a new filter
            return *this;
        }

        ~ScalableStandardBloomFilter() {
        }
};
    
    
    class GenericScalableBloomFilter : public DynamicFilter, public ScalableBase {
        // vector<shared_ptr<StandardBloomFilter>> filters;
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each StandardBloomFilter
        uint32_t s; // Scalable growth factor
        time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        // :TODO
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * pow(r, filters.size());
            uint32_t capacity = p * pow(s, filters.size());
            
            uint32_t k_ = 0;
            k_ = (!filters.empty()) ? TightenedK(filters.front()->K(),filters.size()-1,r) : 0;

            auto new_filter = filters.back()->Duplicate(capacity, fpRate, k_);
            filters.push_back(make_shared<StandardBloomFilter>(capacity, 1, fpRate, k_));
            // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
            return 0;
        }
    public:
        GenericScalableBloomFilter(shared_ptr<StaticFilter> subfilter) : r   (Defaults::TIGHTENING_RATIO),
                                        fp  (Defaults::FALSE_POSITIVE_RATE),
                                        p   (Defaults::MAX_COUNT_NUMBER),
                                        s   (Defaults::SCALABLE_GROWTH)
        {
            cout << "ScaleStdBF created without params.\n";
            // :TODO
            filters.push_back(subfilter);
            // AddFilter(); // Add the first filter
        }
        
        string getFilterName() const {
            return "ScalableStandardBloomFilter";
        }

        string getFilterCode() const {
            return "ScalableSBF";
        }


        std::string getConfigure() {
            std::string res = "_ _ _ CSBF Scope _ _ _ \n";
            res += "Tightening-ratio: " + std::__cxx11::to_string(r) + "\n";
            res += "False positive rate: " + std::__cxx11::to_string(FPrate()) + "\n";
            res += "Current capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
            res += "Current filter size: " + std::__cxx11::to_string(Size()) + "\n";
            res += "Scale growth: " + std::__cxx11::to_string(s) + "\n";
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
        GenericScalableBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (filters.empty() || std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
                AddFilter(); // Add a new filter if all filters are full
            }
            // cout << "csbf-Adding: " << data.data() << endl;
            // cout << filters.back().Size() << endl;
            // cout << filters.back().Capacity() << endl;

            filters.back()->Add(data); // Add data to the last filter
            // cout << "csbf-Added: " << data.data() << endl;
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

        // Resets the filter to its original state.
        // Returns a reference to the filter for chaining.
        GenericScalableBloomFilter& Reset() {
            filters.clear(); // Clear the filter list
            // AddFilter(); // Add a new filter
            return *this;
        }

        ~GenericScalableBloomFilter() {
        }
};

    //: See if each class can be made with a template. The templated parameters should include: <Bloom Filter Class>, Initial Capacity, Num Of Bit, Desired FPRate
}