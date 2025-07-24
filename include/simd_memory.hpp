#pragma once

#include "vector_register.hpp"
#include "mask_register.hpp"
#include <cmath>

namespace qlm
{
    namespace vec
    {
        /*
            linear load/store function that loads data from a pointer into a VecRegister
        */
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Load(const T*const ptr, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            qlm::VecRegister<T, simd_size> vec;
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    vec[i] = ptr[i];
            }
            return vec;
        }

        template<int simd_size, Primitive dst_t, Primitive src_t>
        requires ValidSIMDWidth<simd_size> && (sizeof(src_t) <= sizeof(dst_t))
        VecRegister<dst_t, simd_size> WidenLoad(const src_t*const ptr, const MaskRegister<MaskSize<dst_t, simd_size>>& mask = MaskRegister<MaskSize<dst_t, simd_size>>(true))
        {
            qlm::VecRegister<dst_t, simd_size> vec;
            constexpr int num_elements = mask.num_elements;
            for (int i = 0; i < num_elements; i++)
            {
                if (mask[i])
                    vec[i] = static_cast<dst_t>(ptr[i]);
            }
            return vec;
        }

        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        void Store(T*const ptr, const VecRegister<T, simd_size>& vec, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    ptr[i] = vec[i];
            }
        }

        /*
            gather load/store function that loads data from a pointer into a VecRegister
        */
        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Gather(const T*const ptr, const VecRegister<int, simd_size>& add, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            qlm::VecRegister<T, simd_size> vec;
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    vec[i] = ptr[add[i]];
            }
            return vec;
        }

        template<int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        void Scatter(T*const ptr, const VecRegister<T, simd_size>& vec, const VecRegister<int, simd_size>& add, const MaskRegister<MaskSize<T, simd_size>>& mask = MaskRegister<MaskSize<T, simd_size>>(true))
        {
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    ptr[add[i]] = vec[i];
            }
        }
    }
}