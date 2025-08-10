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
void CompareAndSwap(qlm::VecRegister<int32_t, simd_size>& min_vec, qlm::VecRegister<int32_t, simd_size>&max_vec) 
{
    const auto temp = min_vec;
    min_vec = qlm::vec::Min(min_vec, max_vec);
    max_vec = qlm::vec::Max(temp, max_vec);
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

void simd_bitonic_sort_stage0(int32_t* arr, const int size, const int stage_group_size) 
{
    // Base indices for even and odd positions
    const auto even_indices = qlm::vec::Ramp<simd_size, int32_t>() * 2;     // [0,2,4,6,8,10,...]
    const auto odd_indices = even_indices + 1;                              // [1,3,5,7,9,11,...]

    // Create alternating mask based on stage_group_size
    // For stage_group_size = 2: [1,0,1,0,1,0,...]
    // For stage_group_size = 4: [1,1,0,0,1,1,0,0,...]
    // For stage_group_size = 8: [1,1,1,1,0,0,0,0,...]
    const auto mask = (qlm::vec::Ramp<simd_size, int32_t>() % stage_group_size) < (stage_group_size / 2);

    // Select indices based on the mask
    const auto min_indices = qlm::vec::Select(mask, even_indices, odd_indices);
    const auto max_indices = qlm::vec::Select(mask, odd_indices, even_indices);

    simd_bitonic_sort_stageN(arr, size, min_indices, max_indices);
}

void simd_bitonic_sort_stage1(int32_t* arr, const int size) 
{   
    const auto base_indices = (qlm::vec::Ramp<simd_size, int32_t>() / 2) *4; // 0 0 4 4 8 8 12 12 ...
    const auto indicies_mask = (qlm::vec::Ramp<simd_size, int32_t>() % 2) == 0; // 0 1 0 1 0 1 0 1 ...
    const auto min_indices = qlm::vec::Select(indicies_mask, base_indices, base_indices + 1); // 0 1 4 5 8 9 12 13 ...
    const auto max_indices = qlm::vec::Select(indicies_mask, min_indices + 3, min_indices + 1); // 3 2 7 6 11 10 15 14 ...

    simd_bitonic_sort_stageN(arr, size, min_indices, max_indices);
    simd_bitonic_sort_stage0(arr, size, 2);
}

void simd_bitonic_sort_stage2(int32_t* arr, const int size) 
{
    // Block size is 8 elements for this butterfly
    const auto base_indices = (qlm::vec::Ramp<simd_size, int32_t>() / 4) * 8;  // 0 0 0 0 8 8 8 8 ...
    const auto inner_offset = qlm::vec::Ramp<simd_size, int32_t>() % 4;        // 0 1 2 3 0 1 2 3 ...

    // Left side = first half ascending
    const auto min_indices = base_indices + inner_offset;                        // 0 1 2 3 8 9 10 11 ...

    // Right side = second half descending
    const qlm::VecRegister<int32_t, simd_size> seven{7};
    const auto max_indices = base_indices + (seven - inner_offset);                   // 7 6 5 4 15 14 13 12 ...

    qlm::vec::Print("Indices 0: ", min_indices);
    qlm::vec::Print("Indices 1: ", max_indices);

    simd_bitonic_sort_stageN(arr, size, min_indices, max_indices);
    PrintArray("After stage 2_0", arr, size);
    simd_bitonic_sort_stage1(arr, size);
    PrintArray("After stage 1_2", arr, size);
}

void simd_bitonic_sort(int32_t* arr, int size) {
    const int num_stages = std::log2(size);
}

#if 0
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
#else
int main() {
    constexpr int array_size = 16;
    int32_t* arr = new int32_t[array_size];

    // Initialize with values from the image (left-most column)
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 5;
    arr[3] = 9;
    arr[4] = 3;
    arr[5] = 8;
    arr[6] = 12;
    arr[7] = 14;
    arr[8] = 90;
    arr[9] = 0;
    arr[10] = 60;
    arr[11] = 40;
    arr[12] = 23;
    arr[13] = 35;
    arr[14] = 95;
    arr[15] = 18;

    // Print initial array
    PrintArray("Initial array", arr, array_size);

    // Run only stage 0
    simd_bitonic_sort_stage0(arr, array_size, 2);
    PrintArray("After stage 0", arr, array_size);


    delete[] arr;
    return 0;
}
#endif