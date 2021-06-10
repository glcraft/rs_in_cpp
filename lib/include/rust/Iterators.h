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
    template <typename T>
    concept container = 
        std::forward_iterator<typename T::iterator> &&
        requires (T a) {
            {a.begin()} -> std::convertible_to<typename T::iterator>;
            {a.end()} -> std::convertible_to<typename T::iterator>;
        };
#endif
    // template <typename TypeFrom, typename TypeTo, typename FnPred>
    //     requires map_function<FnPred, TypeFrom, TypeTo>
    // struct Map;

    template <typename T>
#ifdef __cpp_lib_concepts
        requires std::forward_iterator<T>
#endif
    struct ForwardIterator : std::forward_iterator_tag
    {
        using iterator_type = T;
        using value_type = typename T::value_type;
        using pointer = value_type*;
        using reference = value_type&;

        // using iterator_category = std::forward_iterator_tag;
        // using difference_type   = std::ptrdiff_t;
        // using value_type        = T;
        // using pointer           = T*;  // or also value_type*
        // using reference         = T&;  // or also value_type&

        ForwardIterator(iterator_type from, iterator_type to) : it(from), end(to)
        {}
        template<container Cont>
        ForwardIterator(Cont& cont) : it(cont.begin()), end(cont.end())
        {}

        ForwardIterator& operator++() {
            ++it;
            return *this;
        }
        std::optional<reference> operator*() {
            if (it != end)
                return *it;
            return std::nullopt;
        }
        pointer operator->() {
            return it.operator->();
        }
        // Map<T,U,Fn> map(predicate: P)
        // {

        // }
    protected: 
        T it, end;
    };
    template<container Cont>
    ForwardIterator(Cont) -> ForwardIterator<typename Cont::iterator>;
    
    template <container Cont>
        requires std::forward_iterator<typename Cont::iterator>
    ForwardIterator<typename Cont::iterator> iter(Cont& cont)
    {
        return ForwardIterator(cont);   
    }
} // namespace rust
