#ifndef ACP_POOL_HPP
#define ACP_POOL_HPP

#include <cstddef>
#include <initializer_list>
#include <new>

class PoolAllocator {
public:
    PoolAllocator(std::size_t const /*block_size*/, std::initializer_list<std::size_t> /*sizes*/);

    void* allocate(std::size_t const /*n*/);

    void deallocate(void const* /*ptr*/);
};

#endif  // ACP_POOL_HPP
