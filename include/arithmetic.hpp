#pragma once

#include "load_store.hpp"


namespace qlm
{
    namespace vec
    {
        /*
            vector addition function that adds two VecRegisters element-wise
        */
        template<int simd_size, Primitive T, int mask_size>
        requires ValidSIMDFunction<T, simd_size, mask_size>
        VecRegister<T, simd_size> Add(const VecRegister<T, simd_size>& vec0, const VecRegister<T, simd_size>& vec1, const MaskRegister<mask_size>& mask)
        {
            qlm::VecRegister<T, simd_size> res;
            for (int i = 0; i < vec0.num_elements; i++)
            {
                if (mask[i])
                    res[i] = vec0[i] + vec1[i];
            }
            return res;
        }

        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Add(const VecRegister<T, simd_size>& vec0, const VecRegister<T, simd_size>& vec1)
        {
            constexpr int num_bits = simd_size / (sizeof(T) * 8);
            const MaskRegister<num_bits> mask(true);
            return Add(vec0, vec1, mask);
        }
    }
}