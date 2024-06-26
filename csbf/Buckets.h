#include <iostream>
#include <cstdlib>
using namespace std;
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
  Buckets() {}
  
  // Constructor
  Buckets(uint32_t count, 
          uint8_t bucketSize) {  
    this->count = count;
    // Calculate the size of the data buffer in bytes
    int dataSize = ((bucketSize + 7) / 8);
    
    this->Data = (uint8_t*)calloc(count, dataSize);

    this->bucketSize = bucketSize;
    // Calculate the maximum value for a bucket
    this->Max = (1 << bucketSize) - 1;
    // cout << "still alive" << endl;
    this->recheck_data();
  }

  void recheck_data() const {
    try {
      for (uint32_t i = 0; i < count; ++i) {

          if (this->Data[i] != 0) 
            throw std::invalid_argument("Cell is not zero");
      }
    } catch (const std::invalid_argument& e) {
      cout << "Invalid argument: " << e.what() << endl;
      return;
    }
    cout << "Cells are correctly initialized!" << endl;
  }
  // Get the maximum value a bucket can hold
  uint8_t MaxBucketValue() { return this->Max; }

  // Increment the value of a bucket
  Buckets& Increment(uint32_t bucket, int delta) {
    // cout << "increment: " << bucket << endl;
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
    int dataSize = ((this->bucketSize + 7) / 8);
    this->Data = (uint8_t*)calloc(count, dataSize);
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

    auto a = (uint32_t)((this->Data[byteIndex] & (bitMask << byteOffset)) >> byteOffset);
    return (uint32_t)((this->Data[byteIndex] & (bitMask << byteOffset)) >> byteOffset);
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
  ~Buckets() {  }
};