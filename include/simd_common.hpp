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
}