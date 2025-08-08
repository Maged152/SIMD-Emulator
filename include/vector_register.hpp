#pragma once
#include"simd_concepts.hpp" 


namespace qlm
{
    template<Primitive data_type, int simd_size>
    requires ValidSIMDWidth<simd_size>
    class VecRegister
    {
        public:
            static constexpr int simd_width = simd_size;
            static constexpr int num_elements = simd_width / (sizeof(data_type) * 8);
            data_type data[num_elements];

        public:
            VecRegister() = default;

            VecRegister(const data_type& val) 
            {
                for (int i = 0; i < num_elements; i++)
                    data[i] = val;
            }

            VecRegister(const VecRegister& other) 
            {
                for (int i = 0; i < num_elements; i++)
                    data[i] = other.data[i];
            }

        public:
            VecRegister& operator=(const VecRegister& other) 
            {
                if (this != &other) {
                    for (int i = 0; i < num_elements; i++)
                        data[i] = other.data[i];
                }
                return *this;
            }

            data_type& operator[](const int i) 
            { 
                return data[i]; 
            }

            const data_type& operator[](const int i) const 
            { 
                return data[i]; 
            }
    };
}