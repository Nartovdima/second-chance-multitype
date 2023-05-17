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
    std::vector<std::size_t> obj_sizes;

    std::vector<std::vector<std::byte>> m_storage;

    std::unordered_map<std::size_t, std::size_t> m_obj_sizes_map;
    std::unordered_map<const std::byte*, std::size_t> m_sizes_map;

    std::byte* find_empty_place(std::size_t const n, std::size_t const block_ind);
};

#endif  // ACP_POOL_HPP
