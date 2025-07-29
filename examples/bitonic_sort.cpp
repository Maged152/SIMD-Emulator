#include "simd_emulator.hpp"
#include "simd_debug.hpp"
#include "example_helper.hpp"


constexpr int simd_size = 256;

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
void simd_bitonic_sort(int32_t* arr, int size) {
    bitonic_sort_sequence(arr, 0, size, true);
}

int main() {
    constexpr int array_size = 64;
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