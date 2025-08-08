#include "simd_emulator.hpp"
#include "simd_debug.hpp"
#include "example_helper.hpp"


constexpr int simd_size = 256;

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

void simd_bitonic_sort_stage0(int32_t* arr, const int size) 
{
    constexpr int elements_per_vector = simd_size / (8 * sizeof(int32_t));
    const int loop_inc = elements_per_vector * 2;
    const int tail = size % loop_inc;
    const int iter_inner_loop = size - tail;
    
    const auto even_indices = qlm::vec::Ramp<simd_size, int32_t>() * 2;
    const auto odd_indices = even_indices + 1;

    for(int i = 0; i < iter_inner_loop; i += loop_inc) 
    {
        // Gather even and odd elements
        auto vec_even = qlm::vec::Gather<simd_size, int32_t>(&arr[i], even_indices);
        auto vec_odd = qlm::vec::Gather<simd_size, int32_t>(&arr[i], odd_indices);

        // Compare and swap
        CompareAndSwap(vec_even, vec_odd);

        // Scatter the results back to the array
        qlm::vec::Scatter<simd_size, int32_t>(&arr[i], vec_even, even_indices);
        qlm::vec::Scatter<simd_size, int32_t>(&arr[i], vec_odd, odd_indices);
    }
    if (tail > 0) 
    {
        // Create mask for remaining elements
        const qlm::MaskRegister<elements_per_vector> mask = qlm::vec::Ramp<simd_size, int32_t>() < (tail / 2);

        // Gather tail elements with mask
        auto vec_even = qlm::vec::Gather<simd_size, int32_t>(&arr[iter_inner_loop], even_indices, mask);
        auto vec_odd = qlm::vec::Gather<simd_size, int32_t>(&arr[iter_inner_loop], odd_indices, mask);

        // Compare and swap
        CompareAndSwap(vec_even, vec_odd);

        // Scatter results with mask
        qlm::vec::Scatter<simd_size, int32_t>(&arr[iter_inner_loop], vec_even, even_indices, mask);
        qlm::vec::Scatter<simd_size, int32_t>(&arr[iter_inner_loop], vec_odd, odd_indices, mask);
    }
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
    arr[0] = 85;
    arr[1] = 30;
    arr[2] = 113;
    arr[3] = 58;
    arr[4] = 13;
    arr[5] = 9;
    arr[6] = 112;
    arr[7] = 106;
    arr[8] = 73;
    arr[9] = 28;
    arr[10] = 78;
    arr[11] = 126;
    arr[12] = 122;
    arr[13] = 24;
    arr[14] = 46;
    arr[15] = 61;

    // Print initial array
    std::cout << "Initial array:\n";
    for (int i = 0; i < array_size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n\n";

    // Run only stage 0
    simd_bitonic_sort_stage0(arr, array_size);

    // Print result after stage 0
    std::cout << "After stage 0:\n";
    for (int i = 0; i < array_size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";

    delete[] arr;
    return 0;
}
#endif