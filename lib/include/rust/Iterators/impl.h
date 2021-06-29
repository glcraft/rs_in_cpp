#pragma once
#include <tuple>
#include <optional>
#include <concepts>
#define RSINCPP_IMPL_FUNCS \
inline bool is_some() { \
    return it.is_some(); \
} \
inline bool is_none() { \
    return !is_some(); \
} \
self_type operator++(int) { \
    auto tmp = *this; \
    this->operator++(); \
    return tmp; \
} 
namespace rust
{
#if defined(__cpp_concepts) && defined(__cpp_lib_concepts)
template <typename T>
    concept container = 
        std::forward_iterator<typename T::iterator> &&
        requires (T a) {
            {a.begin()} -> std::convertible_to<typename T::iterator>;
            {a.end()} -> std::convertible_to<typename T::iterator>;
        };
    template <typename Fn, typename Input ,typename Output>
    concept map_function =
        requires (Fn fn_map, Input value) {
            {fn_map(value)} -> std::convertible_to<Output>;
        };
    template <typename Fn, typename T>
    concept predicate_function =
        requires (Fn fn_pred, const T& value) {
            {fn_pred(value)} -> std::convertible_to<bool>;
        };
    template<typename Iter>
    concept iterator = 
        requires (Iter it) {
            {++it} -> std::convertible_to<Iter&>;
            {*it} -> std::convertible_to<std::optional<typename Iter::output_type>>;
        };
#   define RSINCPP_REQUIRES(c) requires c
#else
#   define RSINCPP_REQUIRES(c) 
#endif
    namespace impl
    {
        template<uint32_t IterType, typename ...Types>
        struct Factory {
            std::tuple<Types...> v;
        };
    }
}