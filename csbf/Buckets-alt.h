#include <iostream>
#include <cstdlib>
using namespace std;
class Buckets
{
  // Data buffer to store the cells
  int *data;
  // Size of each cell in bits
  int cellSize;
  // Maximum value a cell can hold
  int max;
  // Number of cells
  long long count;
  long long dataSize;

  long long index_validate(long long index) const
  {
    if (index >= 0 && index < count)
    {
      return index;
    }
    else
    {
      throw std::out_of_range("Index out of bounds");
    }
  }

  // Function to check for overflow in multiplication
  int safe_multiply(size_t num, size_t size, size_t *result)
  {
    if (num == 0 || size == 0)
    {
      *result = 0;
      return 1; // Safe multiplication result
    }
    if (num > SIZE_MAX / size)
    {
      return 0; // Overflow detected
    }
    *result = num * size;
    return 1; // Safe multiplication result
  }

  void *safe_calloc(size_t num, size_t size)
  {
    size_t total_size;
    if (!safe_multiply(num, size, &total_size))
    {
      // Handle overflow
      fprintf(stderr, "Memory allocation size overflow\n");
      return NULL;
    }
    return calloc(num, size);
  }

public:
  Buckets(long long c, int s) : count(c), cellSize(s)
  {
    // Calculate the size of the data buffer in bytes
    // this->dataSize = ((count * cellSize + 7) / 8);
    this->data = (int *)safe_calloc(count, sizeof(int));
    if (this->data == nullptr)
    {
      throw std::bad_alloc(); // Handle allocation failure
    }
    // Calculate the maximum value for a cell
    this->max = (1 << cellSize) - 1;
  };
  int MaxBucketValue()
  {
    return this->max;
  };
  Buckets &Increment(long long index, int delta)
  {
    index = index_validate(index);
    int *index_ptr = &data[index];
    cout << "Index: " << index << " Value: " << *index_ptr << " Delta: " << delta << endl;
    if (*index_ptr + delta <= max)
    {
      *index_ptr += delta;
    }
    else
    {
      *index_ptr = max;
    }
    return *this;
  };
  int Get(long long index) const
  {
    index = index_validate(index);
    return data[index];
  };
  bool Set(long long index, int value)
  {
    index = index_validate(index);
    if (value >= 0 && value <= max)
    {
      data[index] = value;
      return true;
    }
    else
    {
      throw std::out_of_range("Value out of bounds");
    }
  };
  Buckets &Reset()
  {
    long long dataSize = ((count * cellSize + 7) / 8);
    this->data = (int *)calloc(dataSize, sizeof(int));
    return *this;
  };
  // Destructor to free the allocated memory
  ~Buckets() { delete[] data; };
};