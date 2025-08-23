#pragma once

#include "vector_register.hpp"
#include "mask_register.hpp"
#include <type_traits>

namespace qlm 
{
    namespace vec 
    {
        // Bitwise AND
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size> && std::is_integral_v<T>
        VecRegister<T, simd_size> And(const VecRegister<T, simd_size>& a, const VecRegister<T, simd_size>& b, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true)) {
            VecRegister<T, simd_size> res;
            for (int i = 0; i < a.num_elements; ++i) {
                if (mask[i]) res[i] = a[i] & b[i];
            }
            return res;
        }

        // Bitwise OR
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size> && std::is_integral_v<T>
        VecRegister<T, simd_size> Or(const VecRegister<T, simd_size>& a, const VecRegister<T, simd_size>& b, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true)) {
            VecRegister<T, simd_size> res;
            for (int i = 0; i < a.num_elements; ++i) {
                if (mask[i]) res[i] = a[i] | b[i];
            }
            return res;
        }

        // Bitwise XOR
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size> && std::is_integral_v<T>
        VecRegister<T, simd_size> Xor(const VecRegister<T, simd_size>& a, const VecRegister<T, simd_size>& b, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true)) {
            VecRegister<T, simd_size> res;
            for (int i = 0; i < a.num_elements; ++i) {
                if (mask[i]) res[i] = a[i] ^ b[i];
            }
            return res;
        }

        // Bitwise NOT
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size> && std::is_integral_v<T>
        VecRegister<T, simd_size> Not(const VecRegister<T, simd_size>& a, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true)) {
            VecRegister<T, simd_size> res;
            for (int i = 0; i < a.num_elements; ++i) {
                if (mask[i]) res[i] = ~a[i];
            }
            return res;
        }
    }
}
