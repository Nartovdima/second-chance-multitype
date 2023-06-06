#include "acp/Pool.hpp"

#include <algorithm>
#include <cassert>

PoolAllocator::PoolAllocator(std::size_t const block_size, std::initializer_list<std::size_t> sizes)
    : obj_sizes(sizes), m_storage(sizes.size(), std::vector<std::byte>(block_size)), m_used_map(sizes.size()) {
    std::size_t block_ind = 0;
    for (const auto& it : sizes) {
        m_used_map[block_ind].resize(block_size / it);
        block_ind++;
    }
}

void* PoolAllocator::allocate(std::size_t const n) {
    std::size_t block_ind = std::distance(obj_sizes.begin(), std::find(obj_sizes.begin(), obj_sizes.end(), n));
    const std::size_t pos = std::distance(m_used_map[block_ind].begin(),
                                          std::find_if(m_used_map[block_ind].begin(), m_used_map[block_ind].end(),
                                                       [](const bool& cell) { return !cell; }));
    if (pos != m_used_map[block_ind].size()) {
        m_used_map[block_ind][pos] = true;
        const auto ptr             = &m_storage[block_ind][pos * n];
        return ptr;
    }
    throw std::bad_alloc{};
}

void PoolAllocator::deallocate(void const* ptr) {
    auto b_ptr = static_cast<const std::byte*>(ptr);
    std::size_t block_ind =
        std::distance(m_storage.begin(),
                      std::find_if(m_storage.begin(), m_storage.end(), [&b_ptr](const std::vector<std::byte>& block) {
                          return (block.data() <= b_ptr && b_ptr < block.data() + block.size());
                      }));
    if (block_ind == m_storage.size()) {
        return;
    }
    const auto begin         = &m_storage[block_ind][0];
    const std::size_t offset = (b_ptr - begin) / obj_sizes[block_ind];
    assert((b_ptr - begin) % obj_sizes[block_ind] == 0);
    if (offset < m_used_map[block_ind].size()) {
        m_used_map[block_ind][offset] = false;
    }
}
