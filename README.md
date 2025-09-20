# dr

![](https://github.com/davreev/dr/actions/workflows/test.yml/badge.svg)

Base library for personal projects and consulting work

## Build

Use CMake to build

> ⚠️ Currently only tested with Clang and GCC. MSVC is not supported.

```sh
mkdir build

# If using a single-config generator (e.g. Ninja, Unix Makefiles)
cmake -S . -B ./build -G <generator> -DCMAKE_BUILD_TYPE=<config> [-DDR_TEST=ON]
cmake --build ./build

# If using a multi-config generator (e.g. Ninja Multi-Config, Xcode)
cmake -S . -B ./build -G <generator> [-DDR_TEST=ON]
cmake --build ./build --config <config>
```

## Usage

Use `FetchContent` to include in another CMake build

```cmake
include(FetchContent)

FetchContent_Declare(
    dr
    GIT_REPOSITORY https://github.com/davreev/dr.git
    GIT_TAG <revision>
)

# Creates target "dr::dr"
FetchContent_MakeAvailable(dr)

# Link other targets e.g.
add_executable(my-app main.cpp)
target_link_libraries(my-app PRIVATE dr::dr)
```

## Dependencies

- [eigen](https://gitlab.com/libeigen/eigen) (MPL 2.0)
- [fmt](https://github.com/fmtlib/fmt) (MIT)
- [unordered_dense](https://github.com/martinus/unordered_dense) (MIT)
- [utest.h](https://github.com/sheredom/utest.h) (Unlicense)

See corresponding module files in `cmake/deps` for version information
