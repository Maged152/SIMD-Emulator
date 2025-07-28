#pragma once

#include <iostream>
#include <random>
#include <cmath>

// image class
class Image {
private:
    uint8_t* data;
    int width;
    int height;
public:
    Image(int width, int height) : width(width), height(height) {
        data = new uint8_t[width * height];
    }

    ~Image() {
        delete[] data;
    }

    uint8_t& At(int x, int y) 
    {
        return data[y * width + x];
    }

    uint8_t At(int x, int y) const
    {
        return data[y * width + x];
    }

    const uint8_t* PtrAt(int x, int y) const
    {
        return &data[y * width + x];
    }

    int Width() const { return width; }
    int Height() const { return height; }

    void RandomInit() 
    {
        std::mt19937 rng(42);
        std::uniform_int_distribution<uint8_t> dist(0, 255);
        for (int y = 0; y < height; y++)
            for (int x = 0; x <width; x++)
                this->At(x, y) = x;//dist(rng);
    }
};

void initialize_data(int* data1, int* data2, int size)
{
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-100, 100);
    for (int i = 0; i < size; ++i) {
        data1[i] = dist(rng);
        data2[i] = dist(rng);
    }
}

void initialize_data(int* data, int size)
{
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-100, 100);
    for (int i = 0; i < size; ++i) {
        data[i] = dist(rng);
    }
}

bool compare_results(const int* simd_res, const int* scalar_res, int size) 
{
    bool correct = true;
    for (int i = 0; i < size; i++) 
    {
        if (simd_res[i] != scalar_res[i]) 
        {
            correct = false;
            std::cout << "Mismatch at index " << i << ": SIMD=" << simd_res[i] << ", Scalar=" << scalar_res[i] << "\n";
        }
    }

    return correct;
}

