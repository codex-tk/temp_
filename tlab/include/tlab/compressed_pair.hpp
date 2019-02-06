/**
 * @file compressed_pair.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-02-05
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_compressed_pair_h__
#define __tlab_compressed_pair_h__

namespace tlab {

template <typename FirstT, typename SecondT,
          bool is_first_empty = __is_empty(FirstT)>
class compressed_pair : private FirstT {
public:
    compressed_pair(void) : FirstT() {}
    compressed_pair(const FirstT &f, const SecondT &s)
        : FirstT(f), _second(s) {}
    compressed_pair(const compressed_pair &rhs)
        : FirstT(rhs.first()), _second(rhs.second()) {}
    compressed_pair(compressed_pair &&rhs)
        : FirstT(std::move(rhs.first())), _second(std::move(rhs._second)) {}

    explicit compressed_pair(const FirstT &f) : FirstT(f) {}
    explicit compressed_pair(const SecondT &s) : FirstT(), _second(s) {}

    FirstT &first(void) noexcept { return (*this); }
    SecondT &second(void) noexcept { return _second; }

    const FirstT &first(void) const noexcept { return (*this); }
    const SecondT &second(void) const noexcept { return _second; }

private:
    SecondT _second;
};

template <typename FirstT, typename SecondT>
class compressed_pair<FirstT, SecondT, false> {
public:
    compressed_pair(void) {}
    compressed_pair(const FirstT &f, const SecondT &s)
        : _first(f), _second(s) {}
    compressed_pair(const compressed_pair &rhs)
        : _first(rhs.first()), _second(rhs.second()) {}
    compressed_pair(compressed_pair &&rhs)
        : _first(std::move(rhs.first())), _second(std::move(rhs.second())) {}
    explicit compressed_pair(const FirstT &f) : _first(f) {}
    explicit compressed_pair(const SecondT &s) : _second(s) {}

    FirstT &first(void) noexcept { return _first; }
    SecondT &second(void) noexcept { return _second; }

    const FirstT &first(void) const noexcept { return _first; }
    const SecondT &second(void) const noexcept { return _second; }

private:
    FirstT _first;
    SecondT _second;
};

} // namespace tlab

#endif