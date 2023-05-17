#include "acp/Pool.hpp"

#include <cassert>

PoolAllocator::PoolAllocator(std::size_t const block_size, std::initializer_list<std::size_t> sizes)
    : obj_sizes(sizes), m_storage(sizes.size(), std::vector<std::byte>(block_size)) {
    std::size_t block_ind = 0;
    std::size_t max_size  = 0;
    for (const auto& it : sizes) {
        max_size += m_storage[0].size() / it;
        m_obj_sizes_map[it] = block_ind;
        block_ind++;
    }
    m_sizes_map.reserve(max_size * 10);
}

std::byte* PoolAllocator::find_empty_place(std::size_t const n, std::size_t const block_ind) {
    std::byte* pos = &m_storage[block_ind][0];
    std::byte* end = m_storage[block_ind].data() + m_storage[block_ind].size();
    while (m_sizes_map.find(pos) != m_sizes_map.end() && m_sizes_map.find(pos)->second != 0 && pos < end) {
        pos += n;
    }
    if (pos + n - 1 < end)
        return pos;
    else
        return nullptr;
}

void* PoolAllocator::allocate(std::size_t const n) {
    std::size_t block_ind = m_obj_sizes_map[n];
    const auto pos        = find_empty_place(n, block_ind);
    if (pos) {
        const auto ptr   = pos;
        m_sizes_map[ptr] = n;
        return ptr;
    }
    throw std::bad_alloc{};
}

void PoolAllocator::deallocate(void const* ptr) {
    auto b_ptr         = static_cast<const std::byte*>(ptr);
    m_sizes_map[b_ptr] = 0;
}
