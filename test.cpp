#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <string>
// #include "test.h"

using namespace std;

class Buckets {
public:
    std::vector<uint8_t> data; // Using uint8_t to store bits
    uint8_t bucketSize; // Size of each bucket in bits
    uint8_t max; // Maximum value a bucket can hold
    uint32_t count; // Number of buckets

    Buckets() : bucketSize(0), max(0), count(0) {}

    Buckets(uint32_t count, uint8_t bucketSize)
        : count(count), bucketSize(bucketSize) {
        if (bucketSize > 8) {
            throw std::invalid_argument("Bucket size must be 8 bits or less");
        }
        max = (1 << bucketSize) - 1;
        size_t dataSize = ((count * bucketSize + 7) / 8); // Calculate size in bytes
        data.resize(dataSize, 0);
    }

    uint8_t get(uint32_t index) const {
        if (index >= count) {
            throw std::out_of_range("Index out of bounds");
        }
        uint32_t bitIndex = index * bucketSize;
        uint32_t byteIndex = bitIndex / 8;
        uint32_t bitOffset = bitIndex % 8;

        uint8_t value = (data[byteIndex] >> bitOffset) & ((1 << bucketSize) - 1);

        if (bitOffset + bucketSize > 8) {
            value |= (data[byteIndex + 1] & ((1 << (bitOffset + bucketSize - 8)) - 1)) << (8 - bitOffset);
        }

        return value;
    }

    void set(uint32_t index, uint8_t value) {
        if (index >= count) {
            throw std::out_of_range("Index out of bounds");
        }
        if (value > max) {
            throw std::invalid_argument("Value exceeds bucket size");
        }

        uint32_t bitIndex = index * bucketSize;
        uint32_t byteIndex = bitIndex / 8;
        uint32_t bitOffset = bitIndex % 8;

        data[byteIndex] &= ~(((1 << bucketSize) - 1) << bitOffset);
        data[byteIndex] |= (value << bitOffset) & 0xFF;

        if (bitOffset + bucketSize > 8) {
            data[byteIndex + 1] &= ~((1 << (bitOffset + bucketSize - 8)) - 1);
            data[byteIndex + 1] |= value >> (8 - bitOffset);
        }
    }

    void recheckData() const {
        for (uint32_t i = 0; i < count; ++i) {
            uint8_t value = get(i);
            if (value != 0) {
                throw std::runtime_error("Data array contains non-zero values");
            }
        }
        std::cout << "Data array is correctly initialized with zero values.\n";
    }
};

void recheckDataTest()
{
    try
    {
        int count = 300000;
        Buckets buckets(count, 4);
        buckets.recheckData();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


int main() {
    // return recheckDataTest();
    cout << ("E00000000011" < "E00000000022");
        

    return 0;
}

