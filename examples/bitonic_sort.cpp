#include "simd_emulator.hpp"
#include "simd_debug.hpp"
#include "example_helper.hpp"


constexpr int simd_size = 256;


void PrintArray(const char* name, const int32_t* arr, int size) 
{
    std::cout << name << ": ";
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
}

/* Scalar bitonic sort*/
void scalar_sort(int32_t* arr, int size) 
{
    std::sort(arr, arr + size);
}

void bitonic_merge(int32_t* arr, int low, int cnt, bool dir) 
{
    if (cnt > 1) {
        int k = cnt / 2;
        for (int i = low; i < low + k; ++i) {
            if (dir == (arr[i] > arr[i + k])) {
                std::swap(arr[i], arr[i + k]);
            }
        }
        bitonic_merge(arr, low, k, dir);
        bitonic_merge(arr, low + k, k, dir);
    }
}


void bitonic_sort_sequence(int32_t* arr, int low, int cnt, bool dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        bitonic_sort_sequence(arr, low, k, true);      // sort in ascending order
        bitonic_sort_sequence(arr, low + k, k, false); // sort in descending order
        bitonic_merge(arr, low, cnt, dir);
    }
}

/* SIMD bitonic sort*/
/*
    This generic SIMD bitonic sort implementation works only if the vector size is equal or less than the repeating pattern.
    Bigger sizes will require additional handling to generate the indicies and loop over them 
*/
void CompareAndSwap(qlm::VecRegister<int32_t, simd_size>& min_vec, qlm::VecRegister<int32_t, simd_size>&max_vec) 
{
    const auto temp = min_vec;
    min_vec = qlm::vec::Min(min_vec, max_vec);
    max_vec = qlm::vec::Max(temp, max_vec);
}

template<int butterfly_size>
void simd_bitonic_sort_butterfly(qlm::VecRegister<int32_t, simd_size>& min_indices, qlm::VecRegister<int32_t, simd_size>&max_indices, int stage_group_size) 
{
    const auto half_butterfly_size = butterfly_size / 2;
    const auto ramp = qlm::vec::Ramp<simd_size, int32_t>();
    const auto group_base = (ramp / half_butterfly_size) * butterfly_size;
    const auto within_pair = ramp % half_butterfly_size;
    
    const auto indices_0 = group_base + within_pair;
    const auto indices_1 = indices_0 + half_butterfly_size;

    const auto mask = (qlm::vec::Ramp<simd_size, int32_t>() % stage_group_size) < (stage_group_size / 2);

    // Select indices based on the mask
    min_indices = qlm::vec::Select(mask, indices_0, indices_1);
    max_indices = qlm::vec::Select(mask, indices_1, indices_0);
}

void simd_bitonic_sort_stageN(int32_t* arr, const int size, const qlm::VecRegister<int32_t, simd_size>& min_indices, const qlm::VecRegister<int32_t, simd_size>& max_indices) 
{
    constexpr int elements_per_vector = simd_size / (8 * sizeof(int32_t));
    const int loop_inc = elements_per_vector * 2;
    const int tail = size % loop_inc;
    const int iter_inner_loop = size - tail;

    for(int i = 0; i < iter_inner_loop; i += loop_inc) 
    {
        // Gather elements
        auto min_vec = qlm::vec::Gather<simd_size, int32_t>(&arr[i], min_indices);
        auto max_vec = qlm::vec::Gather<simd_size, int32_t>(&arr[i], max_indices);

        // Compare and swap
        CompareAndSwap(min_vec, max_vec);

        // Scatter the results back to the array
        qlm::vec::Scatter<simd_size, int32_t>(&arr[i], min_vec, min_indices);
        qlm::vec::Scatter<simd_size, int32_t>(&arr[i], max_vec, max_indices);
    }
    if (tail > 0) 
    {
        // Create mask for remaining elements
        const qlm::MaskRegister<elements_per_vector> mask = qlm::vec::Ramp<simd_size, int32_t>() < (tail / 2);

        // Gather tail elements with mask
        auto min_vec = qlm::vec::Gather<simd_size, int32_t>(&arr[iter_inner_loop], min_indices, mask);
        auto max_vec = qlm::vec::Gather<simd_size, int32_t>(&arr[iter_inner_loop], max_indices, mask);

        // Compare and swap
        CompareAndSwap(min_vec, max_vec);

        // Scatter results with mask
        qlm::vec::Scatter<simd_size, int32_t>(&arr[iter_inner_loop], min_vec, min_indices, mask);
        qlm::vec::Scatter<simd_size, int32_t>(&arr[iter_inner_loop], max_vec, max_indices, mask);
    }
}

void simd_bitonic_sort_stage0(int32_t* arr, const int size, int stage_group_size = 2) 
{
    qlm::VecRegister<int32_t, simd_size> min_indices, max_indices;
    simd_bitonic_sort_butterfly<2>(min_indices, max_indices, stage_group_size);

    simd_bitonic_sort_stageN(arr, size, min_indices, max_indices);
}

void simd_bitonic_sort_stage1(int32_t* arr, const int size, int stage_group_size = 4) 
{
    qlm::VecRegister<int32_t, simd_size> min_indices, max_indices;
    simd_bitonic_sort_butterfly<4>(min_indices, max_indices, stage_group_size);
    
    simd_bitonic_sort_stageN(arr, size, min_indices, max_indices);
    simd_bitonic_sort_stage0(arr, size, stage_group_size);
}

void simd_bitonic_sort_stage2(int32_t* arr, const int size, int stage_group_size = 8) 
{
    qlm::VecRegister<int32_t, simd_size> min_indices, max_indices;
    simd_bitonic_sort_butterfly<8>(min_indices, max_indices, stage_group_size);
    
    simd_bitonic_sort_stageN(arr, size, min_indices, max_indices);
    simd_bitonic_sort_stage1(arr, size, stage_group_size);
}

void simd_bitonic_sort_stage3(int32_t* arr, const int size, int stage_group_size = 16) 
{
    qlm::VecRegister<int32_t, simd_size> min_indices, max_indices;
    simd_bitonic_sort_butterfly<16>(min_indices, max_indices, stage_group_size);
    
    simd_bitonic_sort_stageN(arr, size, min_indices, max_indices);
    simd_bitonic_sort_stage2(arr, size, stage_group_size);
}

void simd_bitonic_sort(int32_t* arr, int size) {
    simd_bitonic_sort_stage0(arr, size);
    simd_bitonic_sort_stage1(arr, size);
    simd_bitonic_sort_stage2(arr, size);
    simd_bitonic_sort_stage3(arr, size);
}


int main() {
    constexpr int array_size = 16;
    int32_t* arr_scalar = new int32_t[array_size];
    int32_t* arr_simd = new int32_t[array_size];

    // initialize input arrays
    initialize_data(arr_scalar, array_size);
    for (int i = 0; i < array_size; ++i) 
    {
        arr_simd[i] = arr_scalar[i];
    }

    // sort data
    scalar_sort(arr_scalar, array_size);
    simd_bitonic_sort(arr_simd, array_size);

    // Compare results
    if (compare_results(arr_simd, arr_scalar, array_size))
        std::cout << "All data match!\n";
    else
        std::cout << "There are mismatches in the results.\n";

    return 0;
}