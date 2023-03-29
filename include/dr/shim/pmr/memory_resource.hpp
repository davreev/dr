#pragma once

// NOTE: C++17 isn't fully supported in the version of libc++ used by Emscripten (as of 3.1.13)
// https://libcxx.llvm.org/Status/Cxx17.html
#ifdef __EMSCRIPTEN__
#define PMR_EXPERIMENTAL
#endif

#ifdef PMR_EXPERIMENTAL

#include <experimental/memory_resource>

namespace std
{
    namespace pmr = experimental::pmr;
}

#else

#include <memory_resource>

#endif