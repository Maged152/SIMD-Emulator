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

/*
    compare operators for VecRegister
*/
template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
auto operator<(const qlm::VecRegister<T, simd_size>& lhs, const qlm::VecRegister<T, simd_size>& rhs)
{
    constexpr int num_elements = qlm::VecRegister<T, simd_size>::num_elements;
    qlm::MaskRegister<num_elements> mask;
    for (int i = 0; i < num_elements; ++i)
        mask[i] = lhs[i] < rhs[i];

    return mask;
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
auto operator<(const qlm::VecRegister<T, simd_size>& lhs, const T& rhs)
{
    qlm::VecRegister<T, simd_size> vec{rhs};
    return lhs < vec;
}