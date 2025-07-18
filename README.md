# SIMD Emulator
A C++ header-only library for emulating SIMD (Single Instruction, Multiple Data) operations in a portable and easy-to-use way. This library provides vector and mask registers, arithmetic and comparison operations, and a familiar API for experimenting with SIMD-style programming on any platform.

## Features
- Emulates common vector instructions: load, store, add, and more.
- Parameterized by any SIMD width and data type.
- Header-only and portable.

## Example Usage

```cpp
#include "simd_emulator.hpp"
#include <iostream>

constexpr int simd_size = 128;
constexpr int num_elements = simd_size / (sizeof(float) * 8);

int main() 
{
    float a[num_elements], b[num_elements];
    for (int i = 0; i < num_elements; i++) 
    {
        a[i] = float(i);
        b[i] = float(num_elements - i);
    }

    // Load data into SIMD registers
    auto va = qlm::vec::Load<simd_size>(a);
    auto vb = qlm::vec::Load<simd_size>(b);

    // Element-wise addition
    auto vsum = qlm::vec::Add(va, vb);

    // Print result
    for (int i = 0; i < num_elements; ++i)
        std::cout << vsum[i] << " ";
    std::cout << std::endl;
}
```

For more complete examples, see the [examples](./examples) directory.

# Build & Targets

## Configure 
    $ cmake -S <source_dir> -B <build_dir>

You can use `presets`

    $ cmake -S <source_dir> --preset <preset_name>

To know the existing presets

    $ cmake -S <source_dir> --list-presets


## Build
    $ cmake --build <build_dir>

## Install
    $ cmake --install <build_dir> --prefix <install_dir>

## Generate Documentations
    $ cmake --build <build_dir> --target documentation

## Examples
    $ cmake --build <build_dir> --target Example_<example_name>

# Contributing
Contributions are welcome! Please feel free to submit a Pull Request or open an Issue to report any bugs or suggest new features.