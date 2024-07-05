# dr

![](https://github.com/davreev/dr/actions/workflows/test.yml/badge.svg)

Base library for personal projects and consulting work

## Build

Use CMake to build

> ⚠️ Currently only tested with Clang and GCC. MSVC is not supported.

```sh
mkdir build

# If using a single-config generator (e.g. Unix Makefiles, Ninja)
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=(Debug|Release|RelWithDebInfo) [-DDR_TEST=ON]
cmake --build ./build

# If using a multi-config generator (e.g. Ninja Multi-Config, Xcode)
cmake -S . -B ./build [-DDR_TEST=ON]
cmake --build ./build --config (Debug|Release|RelWithDebInfo)
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
