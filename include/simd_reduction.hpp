#pragma once

#include "simd_memory.hpp"
#include <cmath>
#include <limits>

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

        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        T ReduceMin(const VecRegister<T, simd_size>& vec, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            T min_val = std::numeric_limits<T>::max();
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                {
                    min_val = std::min(vec[i], min_val);
                }
            }
            return min_val;
        }

        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        T ReduceMax(const VecRegister<T, simd_size>& vec, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            T max_val = std::numeric_limits<T>::min();
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                {
                    max_val = std::max(vec[i], max_val);
                }
            }
            return max_val;
        }
    }
}