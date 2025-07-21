#pragma once
#include <type_traits>
#include <concepts>

namespace qlm
{
    // Concept to restrict T to primitive types (int, float, etc.)
    template<typename T>
    concept Primitive = std::is_arithmetic_v<T>;

    // Concept to ensure number of bits is a power of two and ≥ 64
    template<int N>
    concept ValidSIMDWidth = (N >= 64) && ((N & (N - 1)) == 0);

    template<int N>
    concept ValidMaskWidth = (N >= 0) && ((N & (N - 1)) == 0);

    template<typename T, int simd_size, int num_bits>
    concept ValidSIMDFunction = ValidSIMDWidth<simd_size> && (num_bits ==  (simd_size / (sizeof(T) * 8)));

    template<typename src_t, typename dst_t, int simd_size, int mask_size>
    concept ValidCastSIMDFunction = ValidSIMDWidth<simd_size> &&
        (mask_size == simd_size / (sizeof(src_t) > sizeof(dst_t) ? sizeof(src_t) * 8 : sizeof(dst_t) * 8));
}