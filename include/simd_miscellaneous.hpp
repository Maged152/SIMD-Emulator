#pragma once

#include "vector_register.hpp"
#include "mask_register.hpp"


namespace qlm
{
    namespace vec
    {
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Permute(const VecRegister<T, simd_size>& vec, const VecRegister<int, simd_size>& indices)
        {
            VecRegister<T, simd_size> res;
            for (int i = 0; i < vec.num_elements; ++i)
            {
                const int idx = indices[i];
                res[i] = (idx >= 0 && idx < vec.num_elements) ? vec[idx] : vec[i];
            }

            return res;
        }

        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Select(const VecRegister<T, simd_size>& vec_true,  const VecRegister<T, simd_size>& vec_false,  const MaskRegister<MaskSize<T, simd_size>>& mask)
        {
            VecRegister<T, simd_size> res;
            for (int i = 0; i < vec_true.num_elements; ++i)
            {
                res[i] = mask[i] ? vec_true[i] : vec_false[i];
            }
            
            return res;
        }

    }
}