# dr

![](https://github.com/davreev/dr/actions/workflows/test.yml/badge.svg)

Assorted utilities

> ⚠️ This library exists mainly to improve code reuse in personal projects. It's likely to undergo
> frequent breaking changes.

## Build

Build with CMake

> ⚠️ Currently only tested with Clang and GCC. MSVC is not supported.

```sh
mkdir build
cd build

# If using a single-config generator (e.g. Unix Makefiles, Ninja)
cmake -DCMAKE_BUILD_TYPE=(Debug|Release|RelWithDebInfo) [-DDR_BENCH=ON] [-DDR_EXAMPLE=ON] [-DDR_TEST=ON] ..
cmake --build .

# If using a multi-config generator (e.g. Ninja Multi-Config, Xcode)
cmake [-DDR_BENCH=ON] [-DDR_EXAMPLE=ON] [-DDR_TEST=ON] ..
cmake --build . --config (Debug|Release|RelWithDebInfo)
```

## Usage

Use `FetchContent` to include in another CMake build

```cmake
include(FetchContent)

FetchContent_Declare(
    dr
    GIT_REPOSITORY https://gitlab.com/davreev/dr.git
    GIT_TAG <revision>
)

# Creates target "dr::dr"
FetchContent_MakeAvailable(dr)

# Link other targets e.g.
add_executable(my-app main.cpp)
target_link_libraries(my-app PRIVATE dr::dr)
```
