#include <iostream>
#include <cstdlib>

int main()
{
    auto a = new uint8_t[1000000];
    uint8_t* array = (uint8_t*)calloc(2000000, sizeof(uint8_t));
    if (array == NULL) {
        // Handle allocation failure
        return 1;
    }

    // Print the addresses correctly
    std::cout << static_cast<void*>(a) << std::endl;
    std::cout << static_cast<void*>(array) << std::endl;

    // Print the first element correctly
    std::cout << array[0]+1 << std::endl;

    // Print the sizes of the pointers
    std::cout << sizeof(a) << std::endl;
    std::cout << sizeof(array) << std::endl;

    free(array);
    delete[] a;

    return 0;
}
