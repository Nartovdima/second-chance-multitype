#ifndef ACP_POOL_HPP
#define ACP_POOL_HPP

#include <cstddef>
#include <initializer_list>
#include <new>
#include <unordered_map>
#include <vector>

class PoolAllocator {
public:
    PoolAllocator(std::size_t const block_size, std::initializer_list<std::size_t> sizes);

    void* allocate(std::size_t const n);

    void deallocate(void const* ptr);

private:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    std::vector<std::size_t> obj_sizes;

    std::vector<std::vector<std::byte>> m_storage;
    std::vector<std::vector<bool>> m_used_map;

    std::size_t find_empty_place(std::size_t const n, std::size_t const block_ind);
};

#endif  // ACP_POOL_HPP
