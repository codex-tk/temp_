/**
 * @file util.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-19
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __tlab_util_h__
#define __tlab_util_h__

#include <tlab/internal.hpp>

namespace tlab {

uint16_t checksum(void *p, int sz);

std::string demangle(const char *name);

std::string hex_str(void *ptr, const int size, const int space,
                    const int crlf);

template <typename T> std::string type_name(const T &t) {
    return demangle(typeid(t).name());
}

template <typename T> class clazz {
public:
    template <typename U> using mem_ptr_t = U T::*;

    template <typename S> struct mem_func_helper;
    template <typename R, typename... Args> struct mem_func_helper<R(Args...)> {
        using type = R (T::*)(Args...);
    };

    template <typename S>
    using mem_func_ptr_t = typename mem_func_helper<S>::type;

    template <typename U>
    static std::ptrdiff_t offset_of(const mem_ptr_t<U> ptr_to_member) {
        const T *const pthis = nullptr;
        const uint8_t *const member = static_cast<const uint8_t *>(
            static_cast<const void *>(&(pthis->*ptr_to_member)));
        std::ptrdiff_t offset(member - static_cast<const uint8_t *>(
                                           static_cast<const void *>(pthis)));
        return offset;
    }

    template <typename U>
    static T *container_of(U *member, const mem_ptr_t<U> ptr_to_member) {
        return static_cast<T *>(static_cast<void *>(
            static_cast<uint8_t *>(static_cast<void *>(member)) -
            offset_of(ptr_to_member)));
    }

    template <typename U>
    inline const T *container_of(const U *member,
                                 const mem_ptr_t<U> ptr_to_member) {
        return static_cast<const T *>(static_cast<const void *>(
            static_cast<const uint8_t *>(static_cast<const void *>(member)) -
            offset_of(ptr_to_member)));
    }
};

// [from boost::intrinsic][[
template <class Parent, class Member>
std::ptrdiff_t offset_of(const Member Parent::*ptr_to_member) {
    const Parent *const parent = nullptr;
    const char *const member = static_cast<const char *>(
        static_cast<const void *>(&(parent->*ptr_to_member)));
    std::ptrdiff_t val(
        member - static_cast<const char *>(static_cast<const void *>(parent)));
    return val;
}

template <class Parent, class Member>
inline Parent *container_of(Member *member,
                            const Member Parent::*ptr_to_member) {
    return static_cast<Parent *>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(member)) -
                            offset_of(ptr_to_member)));
}

template <class Parent, class Member>
inline const Parent *container_of(const Member *member,
                                  const Member Parent::*ptr_to_member) {
    return static_cast<const Parent *>(static_cast<const void *>(
        static_cast<const char *>(static_cast<const void *>(member)) -
        offset_of(ptr_to_member)));
}
// ]]
} // namespace tlab

#endif