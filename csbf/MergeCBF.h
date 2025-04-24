#pragma once
#include "../utils/Utils.h"
#include "BaseBF.h"

using namespace std;
namespace BloomFilterModels {

// template <typename staticFilterType, typename = std::enable_if_t<std::is_base_of_v<StaticFilter, staticFilterType>>>
class MergeableCountingBloomFilter : public DynamicFilter
{
    uint32_t g; // const number of bit array
    vector<shared_ptr<Buckets>> barrs;
    shared_ptr<Buckets> orBarr;
public:
/*
    @ Capabilities: 
    @   - Keep multiple initialized BFs, or initalize with given data
    @   - Only Initial Setting up, no after Set-up insertion should also be allowed
    @   - Membership testing for 1 element accross the BFs
*/

    void Init(
        uint32_t n, // estimated total size of data
        uint8_t b = Defaults::BUCKET_SIZE,
        double fpRate = Defaults::FALSE_POSITIVE_RATE,
        uint32_t k = 0,
        uint32_t countExist = 0,
        string algorithm = Defaults::HASH_ALGORITHM,
        string scheme = Defaults::HASH_SCHEME)
    {
        this->k = 3; // paper-advised configuration
        this->fpRate = 0.05; // paper-advised configuration
        this->m = BloomFilterApp::Utils::OptimalM_perKCounting(n, fpRate, k);
        this->count = countExist;
        this->maxCapacity = n;
        this->algorithm = algorithm;
        this->scheme = scheme;
        this->hashGen = new Hash32::HashGen{k, m};
        
        this->g = 16; // paper-advised configuration (equivalent to 4 bit counter of CBF)
        this->orBarr = make_shared<Buckets>(m, b);
        for (int i=0; i<g; i++) {
            barrs.push_back(make_shared<Buckets>(m, b));
        }
    }

    uint32_t getCuckooHash (uint32_t value) {
        // Convert uint32_t to bytes for reuse with existing logic
        std::vector<uint8_t> data(4);
        data[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
        data[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
        data[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
        data[3] = static_cast<uint8_t>(value & 0xFF);

        return hashGen->Execute(data, Hash32::ALGO_JENKINS)[0] % g;
    }

    /*
        Iterate through the bit array and find the first 0 bit, when found set it to 1 then return its index
        If not loop for maximum of {g} times, then return the last index.
    */
    uint32_t doingVirtualCuckoo(uint32_t cellIndex, uint32_t barrIndex=1, uint32_t depth=1) 
    {
        if (barrs[barrIndex]->Get(cellIndex) == 1) {
            if (depth < g) return doingVirtualCuckoo(cellIndex, getCuckooHash(barrIndex), ++depth);
            else return barrIndex;
        } else {
            barrs[barrIndex]->Set(cellIndex, 1);
            return barrIndex;
        }
    }
    
    MergeableCountingBloomFilter &Add(const std::vector<uint8_t> &data)
    {
        vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);
        // Set the K bits in the bucket array
        for (uint32_t hash : hashes)
        {
            uint32_t activatedIndex = doingVirtualCuckoo(hash);
            orBarr->Set(hash, 1);
        }
        this->count++;
        return *this;
    }
    
    bool Test(const vector<uint8_t>& data) const {
        vector<uint32_t> hashes = hashGen->Execute(data, algorithm, scheme);
        for (uint32_t hash : hashes) {
            if (orBarr->Get(hash) == 0) {
                return false;
            }
        }
        return true;
    }

    /*
        Merging this MergCBF with the *target, make changes on this, then return this.
        This bit array size (m) must be larger than that of the *target.
    */
    MergeableCountingBloomFilter* Combine(MergeableCountingBloomFilter* target) {
        uint32_t sumCapacity = this->Capacity() + target->Capacity();
        if (sumCapacity > this->Capacity()) {
            cout << this->getFilterCode() << ": [DENIED] Merging denied for exceeding capacity.\n";
            return this;
        } else if (this->g != target->g || this->m < target->m) {
            cout << this->getFilterCode() << ": [DENIED] Merging denied for incompatible insides.\n";
            return this;
        }
        for (int i=0; i<g; i++) {
            for (int j=0; j<target->m; j++) {
                Buckets* curBarr = this->barrs[i].get();
                Buckets* tarBarr = target->barrs[i].get();
                uint32_t orResult = curBarr->Get(j) || tarBarr->Get(j);
                curBarr->Set(j, orResult);
                if (i == 0) orBarr->Set(j, orResult);
            }
        }
    }

    string getFilterCode() const {
        return "MergCBF";
    }

    string getFilterName() const {
        return "MergeableCountingBloomFilter";
    }

    std::string getConfigure() {
        std::string res = "_ _ _ CSBF Scope _ _ _ \n";
        res += "False positive rate: " + std::__cxx11::to_string(FPrate()) + "\n";
        res += "Current capacity: " + std::__cxx11::to_string(Capacity()) + "\n";
        res += "Current filter size: " + std::__cxx11::to_string(Size()) + "\n";
        // res += "CBF Scope \n";
        // res += "Number of filters: " + std::__cxx11::to_string(filters.size()) + "\n";
        
        // for (int i=0; i<filters.size(); i++) {
        //     auto filter = filters[i];
        //     res += "_ _ _ Filter " + std::__cxx11::to_string(i) + " _ _ _\n";
        //     res += filter->getConfigure();
        // }
        return res;
    }

    // Returns the maximum capacity of the filter->
    uint32_t Capacity() const {
        // uint32_t c = 0;
        // for (const auto& filter : filters) {
        //         c += filter->Capacity();
        // }
        return maxCapacity;
    }

    // Return False Positive Rate
    double FPrate() const {
        return fpRate;
    }

    // Returns the current filter size.
    uint32_t Size() const {
        return m * g;
    }

    uint32_t K() const {
        return k;
    }

    uint32_t Count() const {
        return count;
    }

};

}