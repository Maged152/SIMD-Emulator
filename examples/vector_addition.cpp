#include <iostream>
#include <random>
#include <cmath>
#include "simd_emulator.hpp"

// Assuming simd_size is 256 bits
constexpr int simd_size = 256;

void initialize_data(int* data1, int* data2, int size)
{
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-100, 100);
    for (int i = 0; i < size; ++i) {
        data1[i] = dist(rng);
        data2[i] = dist(rng);
    }
}

void scalar_add(const int* data1, const int* data2, int* scalar_sum, int size) 
{
    for (int i = 0; i < size; ++i)
        scalar_sum[i] = data1[i] + data2[i];
}

void simd_add(const int* data1, const int* data2, int* simd_sum, const int size) 
{
    constexpr int elements_per_vector = simd_size / (sizeof(int) * 8);
    const int iter_inner_loop = size / elements_per_vector;
    const int tail = size % elements_per_vector; 

    // main loop
    for (int i = 0; i < iter_inner_loop; i++) 
    {
        // load input vectors
        qlm::VecRegister<int, simd_size> vec1 = qlm::vec::Load<simd_size>(&data1[i * elements_per_vector]);
        qlm::VecRegister<int, simd_size> vec2 = qlm::vec::Load<simd_size>(&data2[i * elements_per_vector]);

        // perform vector addition
        const qlm::VecRegister<int, simd_size> vec_sum = qlm::vec::Add(vec1, vec2);

        // store result
        qlm::vec::Store(&simd_sum[i * elements_per_vector], vec_sum);
    }
    // handle tail
    if (tail > 0)
    {
        // create a mask for the tail
        const qlm::MaskRegister<elements_per_vector> mask = qlm::vec::Ramp<simd_size, int>() < tail;

        // load input vectors for the tail
        qlm::VecRegister<int, simd_size> vec1 = qlm::vec::Load<simd_size>(&data1[iter_inner_loop * elements_per_vector], mask);
        qlm::VecRegister<int, simd_size> vec2 = qlm::vec::Load<simd_size>(&data2[iter_inner_loop * elements_per_vector], mask);

        // perform vector addition
        const qlm::VecRegister<int, simd_size> vec_sum = qlm::vec::Add(vec1, vec2);

        // store result
        qlm::vec::Store(&simd_sum[iter_inner_loop * elements_per_vector], vec_sum, mask);
    }
}

bool compare_results(const int* simd_sum, const int* scalar_sum, int size) 
{
    bool correct = true;
    for (int i = 0; i < size; i++) 
    {
        if (simd_sum[i] != scalar_sum[i]) 
        {
            correct = false;
            std::cout << "Mismatch at index " << i << ": SIMD=" << simd_sum[i] << ", Scalar=" << scalar_sum[i] << "\n";
        }
    }

    return correct;
}


int main()
{
    // in/out arrays
    const int arr_size = 250;
    int *in0 = new int [arr_size];
    int *in1 = new int [arr_size];
    int *out_scalar = new int [arr_size];
    int *out_vectorized = new int [arr_size];

    // initialize input arrays
    initialize_data(in0, in1, arr_size);

    // call scalar and vectorized functions
    scalar_add(in0, in1, out_scalar, arr_size);
    simd_add(in0, in1, out_vectorized, arr_size);

    // compare results
    if (compare_results(out_vectorized, out_scalar, arr_size))
    {
        std::cout << "All results match!\n";
    }
    else
    {
        std::cout << "There are mismatches in the results.\n";
    }

    // clean up
    delete[] in0;
    delete[] in1;
    delete[] out_scalar;
    delete[] out_vectorized;
}