#pragma once

#include "vector_register.hpp"
#include "mask_register.hpp"

#include "simd_arithmetic.hpp"


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

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
auto operator==(const qlm::VecRegister<T, simd_size>& lhs, const qlm::VecRegister<T, simd_size>& rhs)
{
    constexpr int num_elements = qlm::VecRegister<T, simd_size>::num_elements;
    qlm::MaskRegister<num_elements> mask;
    for (int i = 0; i < num_elements; ++i)
        mask[i] = lhs[i] == rhs[i];

    return mask;
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
auto operator==(const qlm::VecRegister<T, simd_size>& lhs, const T& rhs)
{
    qlm::VecRegister<T, simd_size> vec{rhs};
    return lhs == vec;
}

// Arithmetic operators using simd_arithmetic.hpp
template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator+(const qlm::VecRegister<T, simd_size>& lhs, const qlm::VecRegister<T, simd_size>& rhs)
{
    return qlm::vec::Add(lhs, rhs);
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator+(const qlm::VecRegister<T, simd_size>& lhs, const T& rhs)
{
    qlm::VecRegister<T, simd_size> vec{rhs};
    return lhs + vec;
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator-(const qlm::VecRegister<T, simd_size>& lhs, const qlm::VecRegister<T, simd_size>& rhs)
{
    return qlm::vec::Sub(lhs, rhs);
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator-(const qlm::VecRegister<T, simd_size>& lhs, const T& rhs)
{
    qlm::VecRegister<T, simd_size> vec{rhs};
    return lhs - vec;
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator*(const qlm::VecRegister<T, simd_size>& lhs, const qlm::VecRegister<T, simd_size>& rhs)
{
    return qlm::vec::Mul(lhs, rhs);
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator*(const qlm::VecRegister<T, simd_size>& lhs, const T& rhs)
{
    qlm::VecRegister<T, simd_size> vec{rhs};
    return lhs * vec;
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator/(const qlm::VecRegister<T, simd_size>& lhs, const qlm::VecRegister<T, simd_size>& rhs)
{
    return qlm::vec::Div(lhs, rhs);
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator/(const qlm::VecRegister<T, simd_size>& lhs, const T& rhs)
{
    qlm::VecRegister<T, simd_size> vec{rhs};
    return lhs / vec;
}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator%(const qlm::VecRegister<T, simd_size>& lhs, const qlm::VecRegister<T, simd_size>& rhs)
{
    return qlm::vec::Mod(lhs, rhs);

}

template<int simd_size, qlm::Primitive T>
requires qlm::ValidSIMDWidth<simd_size>
qlm::VecRegister<T, simd_size> operator%(const qlm::VecRegister<T, simd_size>& lhs, const T& rhs)
{
    qlm::VecRegister<T, simd_size> vec{rhs};
    return lhs % vec;
}
