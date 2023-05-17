#ifndef ACP_CACHE_HPP
#define ACP_CACHE_HPP

#include <cstddef>
#include <new>
#include <ostream>
#include <list>

template <class Key, class KeyProvider, class Allocator>
class Cache {
public:
    template <class... AllocArgs>
    Cache(const std::size_t cache_size, AllocArgs &&...alloc_args)
        : m_max_size(cache_size), m_alloc(std::forward<AllocArgs>(alloc_args)...) {}

    std::size_t size() const { return m_queue.size(); }

    bool empty() const { return m_queue.empty(); }

    template <class T>
    T &get(const Key &key);

    std::ostream &print(std::ostream &strm) const;

    friend std::ostream &operator<<(std::ostream &strm, const Cache &cache) { return cache.print(strm); }

private:
    const std::size_t m_max_size;
    Allocator m_alloc;
    std::list<std::pair<KeyProvider *, bool>> m_queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T &Cache<Key, KeyProvider, Allocator>::get(const Key & key) {
    auto find_elem_pos = std::find_if(m_queue.begin(), m_queue.end(),
                           [&key](const std::pair<KeyProvider *, bool> & q_element) { return *(q_element.first) == key; });
    if (find_elem_pos != m_queue.end()) {
        find_elem_pos->second = true;
        return *reinterpret_cast<T *>(find_elem_pos->first);
    }
    if (m_queue.size() == m_max_size) {
        while (m_queue.back().second != 0) {
            auto tmp   = m_queue.back();
            tmp.second = false;
            m_queue.pop_back();
            m_queue.push_front(tmp);
        }
        m_alloc.template destroy<KeyProvider>(m_queue.back().first);
        m_queue.pop_back();
    }
    

    m_queue.push_front({m_alloc.template create<T>(key), false});
    return *reinterpret_cast<T *>(m_queue.front().first);
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream &Cache<Key, KeyProvider, Allocator>::print(std::ostream &strm) const {
    return strm << "<empty>\n";
}

#endif  // ACP_CACHE_HPP
