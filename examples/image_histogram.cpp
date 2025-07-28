#include "simd_emulator.hpp"
#include "simd_debug.hpp"
#include "example_helper.hpp"

// Assuming simd_size is 256 bits
constexpr int simd_size = 256;

// Scalar histogram calculation
void scalar_histogram(const Image& img, int32_t* hist) 
{
    std::fill(hist, hist + 256, 0);
    for (int y = 0; y < img.Height(); y++)
        for (int x = 0; x <img.Width(); x++)
            hist[img.At(x, y)]++;
}

// SIMD histogram calculation
void simd_histogram(const Image& img, int32_t* hist)
{
    constexpr int elements_per_vector = simd_size / (sizeof(int32_t) * 8);

    // temporary buffer used to store histogram data
    int32_t* hist_buffer = new int32_t[256 * 256];

    // initialize histogram with zeros, it's multiple of 2 so no tail is possible
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x += elements_per_vector) 
        {
            const qlm::VecRegister<int32_t, simd_size> zero{0};
            qlm::vec::Store(&hist_buffer[y * 256 + x], zero);
        }
    }

    // calculate histogram of input image
    const int iter_inner_loop = img.Width() / elements_per_vector;
    const int tail = img.Width() % elements_per_vector;

    // gather/scatter addresses
    const qlm::VecRegister<int32_t, simd_size> val (256);
    const qlm::VecRegister<int32_t, simd_size> one{1};

    for (int y = 0; y < img.Height(); y++) 
    {
        for (int i = 0; i < iter_inner_loop; i++) 
        {
            // load input vector
            qlm::VecRegister<int32_t, simd_size> vec = qlm::vec::WidenLoad<simd_size, int32_t>(img.PtrAt(i * elements_per_vector, y));

            // prepare addresses for histogram
            vec = qlm::vec::Mul(val, vec);
            vec = qlm::vec::Add(qlm::vec::Ramp<simd_size, int32_t>(), vec);

            // increment histogram bins
            qlm::VecRegister<int32_t, simd_size> hist_data = qlm::vec::Gather<simd_size>(hist_buffer, vec);
            hist_data = qlm::vec::Add(hist_data, one);
            qlm::vec::Scatter(hist_buffer, hist_data, vec);
        }
    }
    // handle tail
    if (tail > 0) 
    {
        // create a mask for the tail
        const qlm::MaskRegister<elements_per_vector> mask = qlm::vec::Ramp<simd_size, int32_t>() < tail;

        for (int y = 0; y < img.Height(); y++) 
        {
            // load input vector for the tail
            qlm::VecRegister<int32_t, simd_size> vec = qlm::vec::WidenLoad<simd_size, int32_t>(img.PtrAt(iter_inner_loop * elements_per_vector, y), mask);

            // prepare addresses for histogram
            vec = qlm::vec::Mul(val, vec);
            vec = qlm::vec::Add(qlm::vec::Ramp<simd_size, int32_t>(), vec);

            // increment histogram bins
            qlm::VecRegister<int32_t, simd_size> hist_data = qlm::vec::Gather<simd_size>(hist_buffer, vec, mask);
            hist_data = qlm::vec::Add(hist_data, one);  
            qlm::vec::Scatter(hist_buffer, hist_data, vec, mask);
        }
    }
    
    // store results back to the histogram
    for (int y = 0; y < 256; y++)
    {
        // sum up all elements in the vector
        hist[y] = 0;
        for (int x = 0; x < 256; x += elements_per_vector) 
        {
            qlm::VecRegister<int32_t, simd_size> vec = qlm::vec::Load<simd_size>(&hist_buffer[y * 256 + x]);
            hist[y] += qlm::vec::ReduceAdd(vec);
        }
    }

    // free temporary buffer
    delete[] hist_buffer;
}

int main() 
{
    const int width = 1920, height = 1080;
    Image img(width, height);

    // Initialize image with random pixel values
    img.RandomInit();

    // Allocate histograms
    int32_t hist_scalar[256];
    int32_t hist_simd[256];

    // Compute histograms
    scalar_histogram(img, hist_scalar);
    simd_histogram(img, hist_simd);

    // Compare results
    if (compare_results(hist_simd, hist_scalar, 256))
        std::cout << "All histogram bins match!\n";
    else
        std::cout << "There are mismatches in the histogram results.\n";

    return 0;
}