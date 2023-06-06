#ifndef ACP_CACHE_HPP
#define ACP_CACHE_HPP

#include <algorithm>
#include <cstddef>
#include <queue>
#include <new>
#include <ostream>

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

    ~Cache() {
        for (auto & item : m_queue) {
            m_alloc.template destroy<KeyProvider>(item.object);
        }
    }
private:
    class Entry {
    public:
        KeyProvider * object;
        bool usage_flag;
    };

    const std::size_t m_max_size;
    Allocator m_alloc;
    std::deque<Entry> m_queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T &Cache<Key, KeyProvider, Allocator>::get(const Key &key) {
    auto find_elem_pos =
        std::find_if(m_queue.begin(), m_queue.end(),
                     [&key](const Entry &q_element) { return *(q_element.object) == key; });
    if (find_elem_pos != m_queue.end()) {
        find_elem_pos->usage_flag = true;
        return *static_cast<T *>(find_elem_pos->object);
    }
    if (m_queue.size() == m_max_size) {
        while (m_queue.back().usage_flag) {
            auto tmp   = m_queue.back();
            tmp.usage_flag = false;
            m_queue.pop_back();
            m_queue.push_front(std::move(tmp));
        }
        m_alloc.template destroy<KeyProvider>(m_queue.back().object);
        m_queue.pop_back();
    }

    m_queue.push_front({m_alloc.template create<T>(key), false});
    return *static_cast<T *>(m_queue.front().object);
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream &Cache<Key, KeyProvider, Allocator>::print(std::ostream &strm) const {
    return strm << "<empty>\n";
}

#endif  // ACP_CACHE_HPP
