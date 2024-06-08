#include <iostream>
#include <cstdlib>
using namespace std;
class Buckets {
  class ByteArray {
    public:
    uint8_t* data;
    long long size;
    ByteArray() : data(nullptr), size(0) {}
    ByteArray(long long n) : size(n) {
      this->data = (uint8_t*)calloc(n, sizeof(uint8_t));
      if (this->data == nullptr) {
          throw std::bad_alloc();  // Handle allocation failure
      }
    }
    uint8_t& operator[](long long i) {
        if (i >= 0 && i < size) {
            return this->data[i];
        } else {
            throw std::out_of_range("Index out of bounds");
        }
    }
    const uint8_t& operator[](long long i) const {
        if (i >= 0 && i < size) {
            return this->data[i];
        } else {
            throw std::out_of_range("Index out of bounds");
        }
    }
    int show(long long i) {
      return static_cast<int>(this->data[i]);
    }
    ~ByteArray() {
      free(this->data);
    }
  };
 public:
  // Data buffer to store the buckets
  ByteArray Data;
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
    // this->Data = existData == nullptr ? new uint8_t[((count * bucketSize + 7) / 8)] : recreateExistData(existData, sizeData);
    this->Data = ByteArray(((count * bucketSize + 7) / 8));
    // auto a = new uint8_t[1000000];
    // cout << a << endl;
    // cout << "is Data created1: " << (this->Data == nullptr) << endl;
    // cout << "is Data created2: " << static_cast<void*>(this->Data) << endl;
    // cout << "is Data created3: " << this->Data[0] << endl;

    this->bucketSize = bucketSize;
    // Calculate the maximum value for a bucket
    this->Max = (1 << bucketSize) - 1;
  }

  uint8_t *recreateExistData(const uint8_t *existData, size_t sizeData)
  {
      uint8_t* newData = new uint8_t[sizeData];
      for (size_t i = 0; i < sizeData; i++)
      {
          newData[i] = existData[i];
      }
      return newData;
  }

  // Get the maximum value a bucket can hold
  uint8_t MaxBucketValue() { return this->Max; }

  // Increment the value of a bucket
  Buckets& Increment(uint32_t bucket, int delta) {
    cout << "increment: " << bucket << endl;
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
    this->Data = ByteArray((this->count * this->bucketSize + 7) / 8);
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

    cout << "bucket-getbits: " << this->Data.size << " ";
                              cout << byteIndex << " ";
                              cout << byteOffset << " ";
                              cout << "checkpoint 1" << endl;
                              cout << (this->Data[0])+1 << " "; //# stuck here
                              cout << "checkpoint 2" << endl;
                              cout << bitMask << endl;

    auto a = (uint32_t)((this->Data[byteIndex] & (bitMask << byteOffset)) >> byteOffset);
    cout << "byteIndex: " << (uint32_t)((this->Data[byteIndex] & (bitMask << byteOffset)) >> byteOffset) << endl;
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