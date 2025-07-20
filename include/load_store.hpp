#pragma once

#include "vector_register.hpp"
#include "mask_register.hpp"


namespace qlm
{
    namespace vec
    {
        /*
            linear load/store function that loads data from a pointer into a VecRegister
        */
        template< int simd_size, Primitive T, int mask_size>
        requires ValidSIMDFunction<T, simd_size, mask_size>
        VecRegister<T, simd_size> Load(const T*const ptr, const MaskRegister<mask_size>& mask)
        {
            qlm::VecRegister<T, simd_size> vec;
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    vec[i] = ptr[i];
            }
            return vec;
        }

        template< int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Load(const T*const ptr)
        {
            constexpr int num_bits = simd_size / (sizeof(T) * 8);
            const MaskRegister<num_bits> mask(true);
            return Load<simd_size>(ptr, mask);
        }

        template< int simd_size, Primitive T, int mask_size>
        requires ValidSIMDFunction<T, simd_size, mask_size>
        void Store(T*const ptr, const VecRegister<T, simd_size>& vec, const MaskRegister<mask_size>& mask)
        {
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    ptr[i] = vec[i];
            }
        }

        template< int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        void Store(T*const ptr, const VecRegister<T, simd_size>& vec)
        {
            constexpr int num_bits = simd_size / (sizeof(T) * 8);
            const MaskRegister<num_bits> mask(true);
            Store(ptr, vec, mask);
        }

        /*
            gather load/store function that loads data from a pointer into a VecRegister
        */
        template< int simd_size, Primitive T, int mask_size>
        requires ValidSIMDFunction<T, simd_size, mask_size>
        VecRegister<T, simd_size> Gather(const T*const ptr, const VecRegister<int, simd_size>& add, const MaskRegister<mask_size>& mask)
        {
            qlm::VecRegister<T, simd_size> vec;
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    vec[i] = ptr[add[i]];
            }
            return vec;
        }

        template< int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        VecRegister<T, simd_size> Gather(const T*const ptr, const VecRegister<int, simd_size>& add)
        {
            constexpr int num_bits = simd_size / (sizeof(T) * 8);
            const MaskRegister<num_bits> mask(true);
            return Gather<simd_size>(ptr, add, mask);
        }

        template< int simd_size, Primitive T, int mask_size>
        requires ValidSIMDFunction<T, simd_size, mask_size>
        void Scatter(T*const ptr, const VecRegister<T, simd_size>& vec, const VecRegister<int, simd_size>& add, const MaskRegister<mask_size>& mask)
        {
            for (int i = 0; i < vec.num_elements; i++)
            {
                if (mask[i])
                    ptr[add[i]] = vec[i];
            }
        }

        template< int simd_size, Primitive T>
        requires ValidSIMDWidth<simd_size>
        void Scatter(T*const ptr, const VecRegister<T, simd_size>& vec, const VecRegister<int, simd_size>& add)
        {
            constexpr int num_bits = simd_size / (sizeof(T) * 8);
            const MaskRegister<num_bits> mask(true);
            Scatter(ptr, vec, add, mask);
        }
    }
}