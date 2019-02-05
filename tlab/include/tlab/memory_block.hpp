/**
 * @file memory_block.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-02-05
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_memory_block_h__
#define __tlab_memory_block_h__

#include <tlab/compessed_pair.hpp>
#include <tlab/internal.hpp>

namespace tlab {

template <typename T, template <typename> class AllocatorT> class memory_block {
public:
    using pointer_type = T *;

    memory_block(void) : _value(nullptr), _size(0) {}

    ~memory_block(void) { release(); }

    explicit memory_block(const std::size_t size)
        : _value(nullptr), _size(size) {
        init();
    }

    memory_block(const memory_block &rhs)
        : _value(rhs._value), _size(rhs._size) {
        std::atomic<int> *cptr = counter();
        if (cptr)
            cptr->fetch_add(1);
    }

    memory_block(memory_block &&rhs) : _value(rhs._value), _size(rhs._size) {
        rhs._value.second() = nullptr;
        rhs._size = 0;
    }

    AllocatorT<T> &allocator(void) { return _value.first(); }

    std::atomic<int> *counter(void) {
        return reinterpret_cast<std::atomic<int> *>(_value.second());
    }

    const std::atomic<int> *counter(void) const {
        return reinterpret_cast<const std::atomic<int> *>(_value.second());
    }

    int use_count(void) const {
        const std::atomic<int> *cptr = counter();
        if (cptr) {
            return cptr->load();
        }
        return 0;
    }

    pointer_type ptr(void) { return _value.second(); }

    std::size_t size(void) { return _size; }

    void swap(memory_block &rhs) {
        std::swap(_size, rhs._size);
        std::swap(_value.second(), rhs._value.second());
    }

private:
    void init(void) {
        pointer_type ptr =
            allocator().allocate(sizeof(std::atomic<int>) + _size);
        new (ptr) std::atomic<int>(1);
        _value.second() = ptr;
    }

    void release(void) {
        std::atomic<int> *cptr = counter();
        if (cptr && cptr->fetch_sub(1) == 1) {
            allocator().deallocate(_value.second(),
                                   sizeof(std::atomic<int>) + _size);
        }
    }

private:
    tlab::compressed_pair<AllocatorT<T>, pointer_type> _value;
    std::size_t _size;
};

template <typename> struct is_shared_block : std::false_type {};
template <typename T, template <typename> class AllocatorT>
struct is_shared_block<memory_block<T, AllocatorT>> : std::true_type {};

template <template <typename T, template <typename> class AllocatorT>
          class BlockT,
          typename T, 
          template <typename> class AllocatorT,
          typename = std::enable_if_t<is_shared_block<BlockT<T,AllocatorT>>::value>>
int block_refs(const BlockT<T,AllocatorT>& block){
    return block.use_count();
}

template <typename T>
int block_refs(const T&){
    return 0;
}

} // namespace tlab

#endif