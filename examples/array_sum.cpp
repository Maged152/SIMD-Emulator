#include <iostream>
#include "simd_emulator.hpp"


int main()
{
    qlm::VecRegister<float, 128> vec1(1.0f);
    qlm::MaskRegister<vec1.num_elements> mask1((unsigned long)0b1010);

    std::cout << "Vector elements: ";
    for (int i = 0; i < vec1.num_elements; i++)
    {
        if (mask1[i])  // Check if the mask bit is set
        std::cout << vec1[i] << " ";
    }
}