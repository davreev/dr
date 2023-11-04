#include <dr/allocator.hpp>

namespace dr
{

void* Allocator::allocate(usize const size, usize const alignment) const
{
    return resource()->allocate(size, alignment);
}

void Allocator::deallocate(void* const ptr, usize const size, usize const alignment) const
{
    return resource()->deallocate(ptr, size, alignment);
}

}