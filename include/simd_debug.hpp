#pragma once

#include "simd_memory.hpp"
#include <iostream>


namespace qlm
{
    namespace vec
    {
        template<typename T, int simd_size>
        void Print(const char* name, const qlm::VecRegister<T, simd_size>& vec)
        {
            std::cout << name << ": [";
            for (int i = 0; i < vec.num_elements - 1; ++i) 
                std::cout << vec[i] << ", ";
            
            std::cout << vec[vec.num_elements - 1] << "]\n";
        } 
    }
}