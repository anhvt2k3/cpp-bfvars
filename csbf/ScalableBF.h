#include "../utils/Utils.h"
#include "CountingBF.h"
#include "DelBF.h"

using namespace std;
namespace BloomFilterModels {

    class CountingScalableBloomFilter : public DynamicFilter {
    vector<shared_ptr<CountingBloomFilter>> filters;
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
        
        filters.push_back(make_shared<CountingBloomFilter>(capacity, 4, fpRate));
        // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
        return 0;
    }
public:
    CountingScalableBloomFilter() : r   (Defaults::FILL_RATIO),
                                    fp  (Defaults::FALSE_POSITIVE_RATE),
                                    p   (Defaults::MAX_COUNT_NUMBER),
                                    s   (Defaults::SCALABLE_GROWTH)
    {
        AddFilter(); // Add the first filter
    }
    
    string getFilterName() const {
        return "CountingScalableBloomFilter";
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
            if (filter->Count() < filter->Capacity()) {
                c += filter->Capacity();
            }
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
        if (std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
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
    
    class StandardCountingScalableBloomFilter : public DynamicFilter {
        vector<shared_ptr<StandardCountingBloomFilter>> filters;
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each StandardCountingBloomFilter
        uint32_t s; // Scalable growth factor
        // time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * pow(r, filters.size());
            uint32_t capacity = p * pow(s, filters.size());
            
            filters.push_back(make_shared<StandardCountingBloomFilter>(capacity, 4, fpRate));
            // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
            return 0;
        }
    public:
        StandardCountingScalableBloomFilter() : r   (Defaults::FILL_RATIO),
                                        fp  (Defaults::FALSE_POSITIVE_RATE),
                                        p   (Defaults::MAX_COUNT_NUMBER),
                                        s   (Defaults::SCALABLE_GROWTH)
        {
            AddFilter(); // Add the first filter
        }
        
        string getFilterName() const {
            return "StandardCountingScalableBloomFilter";
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
                if (filter->Count() < filter->Capacity()) {
                    c += filter->Capacity();
                }
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
        StandardCountingScalableBloomFilter& Add(const std::vector<uint8_t>& data) {
            if (std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
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
        StandardCountingScalableBloomFilter& Reset() {
            filters.clear(); // Clear the filter list
            AddFilter(); // Add a new filter
            return *this;
        }

        ~StandardCountingScalableBloomFilter() {
        }

};

    class ScalableDeletableBloomFilter : public DynamicFilter {
        vector<shared_ptr<DeletableBloomFilter>> filters;
        double r; // Tightening ratio
        double fp; // Target false-positive rate
        uint32_t p; // Maximum item count for each DeletableBloomFilter
        uint32_t s; // Scalable growth factor
        // time_t syncDate; // Synchronization date

        // Adds a new filter to the list with restricted false-positive rate.
        int AddFilter(const vector<vector<uint8_t>>& data = {}) {
            // Calculate false-positive rate and capacity for the new filter
            double fpRate = fp * pow(r, filters.size());
            uint32_t capacity = p * pow(s, filters.size());
            
            filters.push_back(make_shared<DeletableBloomFilter>(capacity, 4, fpRate));
            // cout << "Filter added " << filters.size()  << " "<< newFilter->Size() << endl;
            return 0;
        }
    public:
        ScalableDeletableBloomFilter() : r   (Defaults::FILL_RATIO),
                                        fp  (Defaults::FALSE_POSITIVE_RATE),
                                        p   (Defaults::MAX_COUNT_NUMBER),
                                        s   (Defaults::SCALABLE_GROWTH)
        {
            AddFilter(); // Add the first filter
        }
        
        string getFilterName() const {
            return "ScalableDeletableBloomFilter";
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
                if (filter->Count() < filter->Capacity()) {
                    c += filter->Capacity();
                }
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
            if (std::all_of(filters.begin(), filters.end(), [](const auto& filter) { return filter->Count() == filter->Capacity(); })) {
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
            AddFilter(); // Add a new filter
            return *this;
        }

        ~ScalableDeletableBloomFilter() {
        }
};
}