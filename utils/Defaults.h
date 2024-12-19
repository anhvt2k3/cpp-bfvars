#include <cstdint>
#include <string>
#include "./hasher/HashGen32.hpp"
#include "./hasher/HashGen64.hpp"

using namespace std;

namespace Defaults {
    const uint8_t BUCKET_SIZE = 1;
    const uint8_t CBF_BUCKET_SIZE = 4;
    const double FALSE_POSITIVE_RATE = 0.002;
    const string HASH_ALGORITHM = string(Hash32::ALGO_MURMUR3_128);
    const string HASH_SCHEME = string(Hash32::SCHEME_ENHANCED_DOUBLE_HASHING);
    
    //# VariableIncrementBF constants
    const uint32_t MIN_INCREMENT = 2;

    //# ScalableBF constants
    const double TIGHTENING_RATIO = 0.8;
    const uint32_t SCALABLE_GROWTH = 2;
    const uint32_t MAX_COUNT_NUMBER = 200000;

    //# DeletableBF constants
    const uint32_t COLLIDE_REGION_SIZE = 1;

    //# CuckooFilter constants
namespace Cuckoo {
    // const uint32_t FINGERPRINT_SIZE = 8;
    const uint32_t BUCKET_CAPACITY = 4;
    const uint32_t FULL_CONDITION = 500;
    };

};