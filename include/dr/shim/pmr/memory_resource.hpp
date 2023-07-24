#pragma once

#ifdef DR_PMR_EXPERIMENTAL

/*
    NOTE: This is a workaround to the current lack of std::pmr namespace in libc++. It allows pmr
    types to be used in a way that's consistent with compliant C++17 standard library
    implementations.

    See https://libcxx.llvm.org/Status/Cxx17.html for details on C++17 compliance in libc++.
*/

#include <experimental/memory_resource>

namespace std
{
namespace pmr = experimental::pmr;
}

#else

#include <memory_resource>

#endif