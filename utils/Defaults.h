#include <cstdint>

namespace Defaults {
    const uint8_t BUCKET_SIZE = 1;
    const uint8_t CBF_BUCKET_SIZE = 4;
    const double FALSE_POSITIVE_RATE = 0.01;
    
    //# ScalableBF constants
    const double FILL_RATIO = 0.8;
    const uint32_t SCALABLE_GROWTH = 2;
    const uint32_t MAX_COUNT_NUMBER = 200000;

    //# DeletableBF constants
    const uint32_t COLLIDE_REGION_SIZE = 100;
};