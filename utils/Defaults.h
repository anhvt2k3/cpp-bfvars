#include <cstdint>

namespace Defaults {
    const uint8_t BUCKET_SIZE = 1;
    const uint8_t CBF_BUCKET_SIZE = 4;
    const double FALSE_POSITIVE_RATE = 0.002;
    
    //# VariableIncrementBF constants
    const uint32_t MIN_INCREMENT = 2;

    //# ScalableBF constants
    const double TIGHTENING_RATIO = 0.8;
    const uint32_t SCALABLE_GROWTH = 2;
    const uint32_t MAX_COUNT_NUMBER = 200000;

    //# DeletableBF constants
    const uint32_t COLLIDE_REGION_SIZE = 2;

    //# CuckooFilter constants
namespace Cuckoo {
    // const uint32_t FINGERPRINT_SIZE = 8;
    const uint32_t BUCKET_CAPACITY = 4;
    const uint32_t FULL_CONDITION = 500;
    };

};