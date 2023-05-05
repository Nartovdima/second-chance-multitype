#include "acp/Pool.hpp"

PoolAllocator::PoolAllocator(std::size_t const, std::initializer_list<std::size_t>) {}

void* PoolAllocator::allocate(std::size_t const) {
    throw std::bad_alloc{};
    //    return std::malloc(sz);
}

void PoolAllocator::deallocate(void const*) {
    //    std::free(const_cast<void*>(ptr));
}
