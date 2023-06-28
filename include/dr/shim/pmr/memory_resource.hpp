#pragma once

#ifdef DR_PMR_EXPERIMENTAL

#include <experimental/memory_resource>

namespace std
{
namespace pmr = experimental::pmr;
}

#else

#include <memory_resource>

#endif