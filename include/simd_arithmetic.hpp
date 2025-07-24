#pragma once

#include "simd_memory.hpp"
#include <functional>


namespace qlm
{
    namespace vec
    {
        /*
            Common element-wise vector operation with mask
            op: a callable with signature T(T, T)
        */
        template<int simd_size, Primitive T, typename Op>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Apply(const VecRegister<T, simd_size>& vec0, const VecRegister<T, simd_size>& vec1, const MaskRegister<MaskSize<T, simd_size>>& mask, Op op)
        {
            qlm::VecRegister<T, simd_size> res;
            for (int i = 0; i < vec0.num_elements; i++)
            {
                if (mask[i])
                    res[i] = op(vec0[i], vec1[i]);
            }
            return res;
        }


        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Add(const VecRegister<T, simd_size>& vec0, const VecRegister<T, simd_size>& vec1, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            return Apply(vec0, vec1, mask, std::plus<T>());
        }


       template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Mul(const VecRegister<T, simd_size>& vec0, const VecRegister<T, simd_size>& vec1, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            return Apply(vec0, vec1, mask, std::multiplies<T>());
        }
    }
}