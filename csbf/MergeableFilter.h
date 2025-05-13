#pragma once
#include "../utils/Utils.h"
#include "BaseBF.h"

using namespace std;
namespace BloomFilterModels {

// template <typename staticFilterType, typename = std::enable_if_t<std::is_base_of_v<StaticFilter, staticFilterType>>>
class MergeableFilter : public DynamicFilter
{
public:
/*
    @ Capabilities: 
    @   - Keep multiple initialized BFs, or initalize with given data
    @   - Not only Initial Setting up, after Set-up insertion should also be allowed
    @   - Membership testing for 1 element accross the BFs
*/
    MergeableFilter() {
        cout << "Mergeable Filter created without params.\n";
    }

    string getFilterCode() const {
        return (!filters.empty())? "MergedFilter."+filters.front()->getFilterCode():"MergedFilter";
    }

    string getFilterName() const {
        return (!filters.empty())? "Mergeable Filter Of "+filters.front()->getFilterName():"Mergeable Filter";
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
        return fpRate;
    }

    // Returns the current filter size.
    uint32_t Size() const {
        long long size = 0;
        cout << "MF: Getting size of "<<filters.size()<<".\n";
        for (const auto& filter : filters) {
            cout << "MF: Getting size of "<<filter->getFilterCode()<<" "<<filter->Size()<<".\n";
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

    int AddFilter(shared_ptr<AbstractFilter> filter) {
        filters.push_back(filter);
        return 0;
    }

    bool Test(const vector<uint8_t>& data) const {
        for (const auto& filter : filters) {
            // Check for membership in each filter
            if (filter->Test(data) == true) {
                    return true;
            }
        }
        return false;
    }

};

}