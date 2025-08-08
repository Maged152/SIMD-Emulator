#pragma once
#include "simd_concepts.hpp"

namespace qlm
{
    template<int simd_size>
    requires ValidMaskWidth<simd_size>
    class MaskRegister
    {
    public:
        static constexpr int num_elements = simd_size;
        bool data[num_elements];

        MaskRegister() = default;

        explicit MaskRegister(const bool val)
        {
            for (int i = 0; i < num_elements; ++i)
                data[i] = val;
        }

        explicit MaskRegister(const unsigned long val)
        {
            for (int i = 0; i < num_elements; ++i)
                data[i] = (val >> i) & 1;
        }

        MaskRegister(const MaskRegister& other)
        {
            for (int i = 0; i < num_elements; ++i)
                data[i] = other.data[i];
        }

        MaskRegister& operator=(const MaskRegister& other)
        {
            if (this != &other) {
                for (int i = 0; i < num_elements; ++i)
                    data[i] = other.data[i];
            }
            return *this;
        }

        bool& operator[](const int i)
        {
            return data[i];
        }

        const bool& operator[](const int i) const
        {
            return data[i];
        }
    };
}