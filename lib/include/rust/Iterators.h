#pragma once
#include <iterator>
namespace rust
{
#ifdef __cpp_lib_concepts
    template <typename Fn, typename T ,typename U>
    concept map_function =
        requires (Fn fn_pred, T value) {
            {fn(value)} -> std::convertible_to<U>;
        };
    template <typename Fn, typename T>
    concept predicate_function =
        requires (Fn fn_pred, const T& value) {
            {fn(value)} -> std::convertible_to<bool>;
        };
#endif
    // template <typename TypeFrom, typename TypeTo, typename FnPred>
    //     requires map_function<FnPred, TypeFrom, TypeTo>
    // struct Map;

    template <typename T>
#ifdef __cpp_lib_concepts
        requires std::forward_iterator<T>
#endif
    struct Iterator : std::forward_iterator_tag
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = int;
        using pointer           = int*;  // or also value_type*
        using reference         = int&;  // or also value_type&

        template <typename U, typename Fn>
#ifdef __cpp_lib_concepts
            requires map_function<Fn,T,U>
#endif
        Iterator& operator++() {
            ++it;
            return *this;
        }
        const Iterator& operator++() {
            ++it;
            return *this;
        }
        // Map<T,U,Fn> map(predicate: P)
        // {

        // }
    protected: 
        T it, end;
    };
} // namespace rust