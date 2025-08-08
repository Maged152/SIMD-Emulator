#pragma once

#include "vector_register.hpp"
#include "mask_register.hpp"


namespace qlm
{
    namespace vec
    {
        template< int simd_size, qlm::Primitive T>
        requires qlm::ValidSIMDWidth<simd_size>
        qlm::VecRegister<T, simd_size> Ramp()
        {
            qlm::VecRegister<T, simd_size> vec;
            for (int i = 0; i < vec.num_elements; i++)
                vec[i] = i;

            return vec;
        }

    }
}