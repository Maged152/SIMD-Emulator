#pragma once

#include "simd_memory.hpp"


namespace qlm
{
    namespace vec
    {
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        T ReduceAdd(const VecRegister<T, simd_size>& vec, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            T sum = 0;
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                {
                    sum += vec[i];
                }
            }
            return sum;
        }
    }
}