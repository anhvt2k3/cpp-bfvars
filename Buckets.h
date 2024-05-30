#include <iostream>
#include <vector>
#include <stdint.h>

class Buckets {
 public:
  // Data buffer to store the buckets
  uint8_t* Data;
  // Size of each bucket in bits
  uint8_t bucketSize;
  // Maximum value a bucket can hold
  uint8_t Max;
  // Number of buckets
  uint32_t count;
  
  // Constructor
  Buckets(uint32_t count, 
          uint8_t bucketSize, 
          const uint8_t* existData, 
          size_t sizeData) {  
    this->count = count;
    // Calculate the size of the data buffer in bytes
    this->Data = existData == nullptr ? new uint8_t[((count * bucketSize + 7) / 8)]
                                      : recreateExistData(existData, sizeData);
    this->bucketSize = bucketSize;
    // Calculate the maximum value for a bucket
    this->Max = (1 << bucketSize) - 1;
  }

  uint8_t *recreateExistData(const uint8_t *existData, size_t sizeData)
  {
    return copy(existData, existData + sizeData, new std::vector<uint8_t>(sizeData))->data();
  }

  // Get the maximum value a bucket can hold
  uint8_t MaxBucketValue() { return this->Max; }

  // Increment the value of a bucket
  Buckets& Increment(uint32_t bucket, int delta) {
    int val = (int)(GetBits(bucket * this->bucketSize, this->bucketSize) + delta);

    if (val > this->Max) {
      val = this->Max;
    } else if (val < 0) {
      val = 0;
    }

    SetBits((uint32_t)bucket * (uint32_t)this->bucketSize, this->bucketSize,
             (uint32_t)val);
    return *this;
  }

  // Set the value of a bucket
  Buckets& Set(uint32_t bucket, uint8_t value) {
    if (value > this->Max) {
      value = this->Max;
    }
    SetBits(bucket * this->bucketSize, this->bucketSize, value);
    return *this;
  }

  // Get the value of a bucket
  uint32_t Get(uint32_t bucket) const {
    return GetBits(bucket * this->bucketSize, this->bucketSize);
  }

  // Reset the buckets to their initial state
  Buckets& Reset() {
    this->Data = new uint8_t[((this->count * this->bucketSize + 7) / 8)];
    return *this;
  }

  // Get bits from the data buffer at a specific offset and length
  uint32_t GetBits(uint32_t offset, int length) const {
    uint32_t byteIndex = offset / 8;
    int byteOffset = (int)(offset % 8);

    if ((byteOffset + length) > 8) {
      int rem = 8 - byteOffset;
      return GetBits(offset, rem) |
             (GetBits((uint32_t)(offset + rem), length - rem) << rem);
    }

    int bitMask = (1 << length) - 1;
    return (uint32_t)((this->Data[byteIndex] & (bitMask << byteOffset)) >>
                     byteOffset);
  }

  // Set bits in the data buffer at a specific offset and length
  void SetBits(uint32_t offset, int length, uint32_t bits) {
    uint32_t byteIndex = offset / 8;
    int byteOffset = (int)(offset % 8);

    if ((byteOffset + length) > 8) {
      int rem = 8 - byteOffset;
      SetBits(offset, (uint8_t)rem, bits);
      SetBits((uint32_t)(offset + rem), length - rem, bits >> rem);
      return;
    }

    int bitMask = (1 << length) - 1;
    this->Data[byteIndex] =
        (uint8_t)((this->Data[byteIndex]) & ~(bitMask << byteOffset));
    this->Data[byteIndex] =
        (uint8_t)((this->Data[byteIndex]) | ((bits & bitMask) << byteOffset));
  }

  // Destructor to free the allocated memory
  ~Buckets() { delete[] Data; }
};