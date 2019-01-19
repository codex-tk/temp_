/**
 * @file mp.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-12
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_mp_h__
#define __tlab_mp_h__

#include <tlab/internal.hpp>

namespace tlab::mp {

template <typename T> using invoke = typename T::type;

// based on std impl
template <std::size_t... S> struct index_sequence {
    static constexpr std::size_t size(void) noexcept { return sizeof...(S); }
};

template <std::size_t N, std::size_t... S>
struct make_index_sequence_linear
    : make_index_sequence_linear<N - 1, N - 1, S...> {};

template <std::size_t... S> struct make_index_sequence_linear<0, S...> {
    using type = index_sequence<S...>;
};

// based on http://stackoverflow.com/a/17426611/410767 by Xeo
template <typename S1, typename S2> struct merge_index_sequence;

template <std::size_t... S1, std::size_t... S2>
struct merge_index_sequence<index_sequence<S1...>, index_sequence<S2...>> {
    using type = index_sequence<S1..., (sizeof...(S1) + S2)...>;
};

template <std::size_t N>
struct make_index_sequence_merge
    : merge_index_sequence<
          typename make_index_sequence_merge<N / 2>::type,
          typename make_index_sequence_merge<N - N / 2>::type> {};

template <> struct make_index_sequence_merge<0> {
    using type = index_sequence<>;
};

template <> struct make_index_sequence_merge<1> {
    using type = index_sequence<0>;
};

template <std::size_t N>
using make_index_sequence = invoke<make_index_sequence_merge<N>>;

template <typename Tl, typename... Ts> struct push_back;

template <template <typename...> class Tl, typename... Ts, typename... Us>
struct push_back<Tl<Ts...>, Us...> {
    using type = Tl<Ts..., Us...>;
};

template <typename Tl, typename... Ts> struct push_front;

template <template <typename...> class Tl, typename... Ts, typename... Us>
struct push_front<Tl<Ts...>, Us...> {
    using type = Tl<Us..., Ts...>;
};

template <std::size_t I, typename Tl1, typename Tl2> struct pop_back_helper;

template <std::size_t I, template <typename...> class Tl1, typename... Ts,
          template <typename...> class Tl2, typename Head, typename... Us>
struct pop_back_helper<I, Tl1<Ts...>, Tl2<Head, Us...>> {
    using type =
        invoke<pop_back_helper<I - 1, invoke<push_back<Tl1<Ts...>, Head>>,
                               Tl2<Us...>>>;
};

template <template <typename...> class Tl1, template <typename...> class Tl2,
          typename Head, typename... Ts, typename... Us>
struct pop_back_helper<1, Tl1<Ts...>, Tl2<Head, Us...>> {
    using type = Tl1<Ts...>;
};

template <typename Tl> struct pop_back;

template <template <typename...> class Tl, typename... Ts>
struct pop_back<Tl<Ts...>> {
    using type = invoke<pop_back_helper<sizeof...(Ts), Tl<>, Tl<Ts...>>>;
};

template <std::size_t I, typename Tl> struct at;

template <std::size_t I, template <typename...> class Tl, typename Head,
          typename... Ts>
struct at<I, Tl<Head, Ts...>> {
    using type = invoke<at<I - 1, Tl<Ts...>>>;
};
template <template <typename...> class Tl, typename Head, typename... Ts>
struct at<0, Tl<Head, Ts...>> {
    using type = Head;
};

template <typename Sl, typename Tl> struct pop_back_helper0;

template <template <std::size_t...> class Sl, std::size_t... Is,
          template <typename...> class Tl, typename... Ts>
struct pop_back_helper0<Sl<Is...>, Tl<Ts...>> {
    using type = Tl<invoke<at<Is, Tl<Ts...>>>...>;
};

template <typename Tl> struct pop_back0;

template <template <typename...> class Tl, typename... Ts>
struct pop_back0<Tl<Ts...>> {
    using type = invoke<
        pop_back_helper0<make_index_sequence<sizeof...(Ts) - 1>, Tl<Ts...>>>;
};

template <typename Tl, template <typename...> class F> struct transform {};

template <template <typename...> class Tl, typename... Ts,
          template <typename...> class F>
struct transform<Tl<Ts...>, F> {
    using type = Tl<invoke<F<Ts>>...>;
};

template <typename Tl> struct size;
template <template <typename...> class Tl, typename... Ts>
struct size<Tl<Ts...>> {
    static constexpr std::size_t value =
        std::integral_constant<std::size_t, sizeof...(Ts)>::value;
};

template <typename... Ts> class type_list {};

template <typename Sl, typename Tl> class zip;

template <template <std::size_t...> class Sl, std::size_t... Is,
          template <typename...> class Tl, typename... Ts>
struct zip<Sl<Is...>, Tl<Ts...>> {
    using type =
        type_list<type_list<std::integral_constant<std::size_t, Is>, Ts>...>;
};

template <typename T> struct type2type { using type = T; };

namespace exp {

template <std::size_t I, typename T> struct storage {
    storage(T &&t) : value(t) {}
    T value;
};

template <typename S, typename... Ts> struct storage_host;
template <std::size_t... S, typename... Ts>
struct storage_host<tlab::mp::index_sequence<S...>, Ts...> : storage<S, Ts>... {
    storage_host(Ts &&... args) : storage<S, Ts>(std::forward<Ts>(args))... {}
};

template <typename... Ts>
using tuple = storage_host<tlab::mp::make_index_sequence<sizeof...(Ts)>, Ts...>;

template <std::size_t I, typename T>
T get(exp::storage<I,T> &v){
    return v.value;
}

} // namespace exp

} // namespace tlab::mp

#ifndef TLAB_PACK_EXPANSION_CALL
#if __cplusplus > 201402L
#define TLAB_PACK_EXPANSION_CALL(expr) ((expr), ...)
#else
#define TLAB_PACK_EXPANSION_CALL(expr)                                         \
    { int dummy[] = {0, (expr, 0)...}; }
#endif
#endif

#endif