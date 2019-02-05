/**
 * @file byte_buffer.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-02-05
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_byte_buffer_h__
#define __tlab_byte_buffer_h__

#include <tlab/memory_block.hpp>

namespace tlab {

template <template <typename, template <typename> class> class BlockT,
          template <typename> class AllocatorT = std::allocator>
class byte_buffer {
public:
    using pointer_type = typename BlockT<char, AllocatorT>::pointer_type;

    byte_buffer(void) : _gpos(0), _ppos(0) {}

    explicit byte_buffer(const std::size_t size)
        : _block(size), _gpos(0), _ppos(0) {}

    byte_buffer(const byte_buffer &rhs)
        : _block(rhs._block), _gpos(rhs._gpos), _ppos(rhs._ppos) {}

    byte_buffer(byte_buffer &&rhs)
        : _block(std::move(rhs._block)), _gpos(rhs._gpos), _ppos(rhs._ppos) {}

    byte_buffer &operator=(const byte_buffer &rhs) {
        byte_buffer nb(rhs);
        swap(nb);
        return *this;
    }

    byte_buffer &operator=(byte_buffer &&rhs) {
        byte_buffer nb(std::forward<byte_buffer>(rhs));
        swap(nb);
        return *this;
    }

    pointer_type gptr(void) { return _block.ptr() + _gpos; }
    int gptr(int seek) {
        seek =
            (seek >= 0 ? std::min(seek, gsize()) : std::max(seek, _gpos * -1));
        _gpos += seek;
        return seek;
    }
    int gsize(void) { return _ppos - _gpos; }

    pointer_type pptr(void) { return _block.ptr() + _ppos; }
    int pptr(int seek) {
        seek = (seek >= 0 ? std::min(seek, psize())
                          : std::max(seek, gsize() * -1));
        _ppos += seek;
        return seek;
    }
    int psize(void) { return static_cast<int>(size()) - _ppos; }

    pointer_type ptr(void) { return _block.ptr(); }
    std::size_t size(void) { return _block.size(); }

    void swap(byte_buffer &rhs) {
        _block.swap(rhs._block);
        std::swap(_gpos, rhs._gpos);
        std::swap(_ppos, rhs._ppos);
    }

    void clear(void) { _gpos = _ppos = 0; }

    void reserve(const int sz) {
        if (sz < 0)
            return;

        if (psize() >= sz)
            return;

        if ((static_cast<int>(size()) - gsize()) >= sz &&
            tlab::block_refs(_block) <= 1) {
            std::memmove(ptr(), gptr(), gsize());
            _ppos = gsize();
            _gpos = 0;
            return;
        }
        byte_buffer nb(gsize() + sz);
        memcpy(nb.pptr(), gptr(), gsize());
        nb.pptr(gsize());
        swap(nb);
    }

private:
    BlockT<char, AllocatorT> _block;
    int _gpos;
    int _ppos;
};

} // namespace tlab

#endif