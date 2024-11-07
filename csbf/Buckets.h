#include <iostream>
#include <cstdlib>
using namespace std;
class Buckets
{
public:
  // Data buffer to store the buckets
  uint8_t *Data;

  // Size of each bucket in bits
  uint32_t bucketSize;

  // Maximum value a bucket can hold
  uint32_t Max;

  // Number of buckets
  uint32_t count;

  // Constructor
  Buckets(uint32_t count,
          uint32_t bucketSize) : bucketSize(bucketSize),
                                 count(count),
                                 Max((1 << bucketSize) - 1)
  {
    this->count = count;
    // Calculate the size of the data buffer in bytes
    int dataSize = ((bucketSize + 8 - 1) / 8);
    this->Data = (uint8_t *)calloc(count, dataSize);

    this->recheck_data();
  }

  Buckets initData(const uint32_t data[], uint32_t n)
  {
    for (uint32_t i = 0; i < n; i++)
    {
      Set(i, data[i]);
    }
    return *this;
  }

  void recheck_data() const
  {
    try
    {
      for (uint32_t i = 0; i < count; i++)
      {

        if (this->Data[i] != 0)
          throw std::invalid_argument("Cell is not zero");
      }
    }
    catch (const std::invalid_argument &e)
    {
      cout << "Invalid argument: " << e.what() << endl;
      return;
    }
    // cout << "Cells are correctly initialized!" << endl;
  }
  // Get the maximum value a bucket can hold
  uint32_t MaxBucketValue() { return this->Max; }

  // Increment the value of a bucket
  Buckets &Increment(uint32_t bucket, int delta)
  {
    int val = (int)(GetBits(uint32_t(bucket * this->bucketSize), this->bucketSize) + delta);

    if (val > this->Max)
    {
      val = this->Max;
    }
    else if (val < 0)
    {
      val = 0;
    }

    SetBits((uint32_t)bucket * (uint32_t)this->bucketSize, this->bucketSize, (uint32_t)val);
    return *this;
  }

  // Set the value of a bucket
  Buckets &Set(uint32_t bucket, int value)
  {
    if (value > this->Max)
    {
      value = this->Max;
    }
    SetBits(uint32_t(bucket * this->bucketSize), this->bucketSize, value);
    return *this;
  }

  // Get the value of a bucket
  uint32_t Get(uint32_t bucket) const
  {
    return GetBits(uint32_t(bucket * this->bucketSize), this->bucketSize);
  }

  // Reset the buckets to their initial state
  Buckets &Reset()
  {
    int dataSize = ((this->bucketSize + 7) / 8);
    this->Data = (uint8_t *)calloc(count, dataSize);
    return *this;
  }

  // Get bits from the data buffer at a specific offset and length
  uint32_t GetBits(uint32_t offset, int length) const
  {
    uint32_t byteIndex = offset / 8;
    int byteOffset = (int)(offset % 8);
    if ((byteOffset + length) > 8)
    {
      int rem = 8 - byteOffset;
      return GetBits(offset, rem) | (GetBits((uint32_t)(offset + rem), length - rem) << rem);
    }

    int bitMask = (1 << length) - 1;
    return (uint32_t)((this->Data[byteIndex] & (bitMask << byteOffset)) >> byteOffset);
  }

  // Set bits in the data buffer at a specific offset and length
  // `offset` is the starting bit index,  `length` is the number of bits to set, `bits` is the value to set
  // `bits` < 2^length else zero will be set
  void SetBits(uint32_t offset, int length, int bits)
  {
    uint32_t byteIndex = offset / 8; //@ 8 is size of uint8_t
    int byteOffset = (int)(offset % 8);

    if ((byteOffset + length) > 8)
    {
      int rem = 8 - byteOffset;
      SetBits(offset, (uint8_t)rem, bits);
      SetBits((uint32_t)(offset + rem), length - rem, bits >> rem);
      return;
    }

    int bitMask = (1 << length) - 1;
    this->Data[byteIndex] =
        (uint8_t)((this->Data[byteIndex]) & ~(bitMask << byteOffset)); //@ strip off the bits that will be replaced
    this->Data[byteIndex] =
        (uint8_t)((this->Data[byteIndex]) | ((bits & bitMask) << byteOffset)); //@ set the value into the blank space
  }

  // Destructor to free the allocated memory
  ~Buckets() {}
};