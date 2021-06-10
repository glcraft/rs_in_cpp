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
    template <typename T>
#ifdef __cpp_lib_concepts
        requires std::forward_iterator<T>
#endif
    struct ForwardIterator;
    template <typename Iter, typename U, class Fn>
    struct Map;

    template <typename T>
#ifdef __cpp_lib_concepts
        requires std::forward_iterator<T>
#endif
    struct ForwardIterator : std::forward_iterator_tag
    {
        using self_type = ForwardIterator<T>;
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
        std::optional<value_type> operator*() {
            if (it != end)
                return *it;
            return std::nullopt;
        }
        pointer operator->() {
            return it.operator->();
        }
        template <class Fn>
        auto map(Fn&& fn)
        {
            using U = std::remove_reference_t<decltype(fn(this->operator*().value()))>;
            return Map<self_type, U, Fn>(*this, std::forward<Fn>(fn));
        }
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
    template <typename Iter, typename U, class Fn>
    struct Map {
        using iterator_type = Iter;
        using value_type = typename Iter::value_type;
        using result_type = std::optional<U>;
        Map(iterator_type it, Fn&& fn): it(it), fn(std::forward<Fn>(fn))
        {}
        result_type operator*() {
            auto value = *it;
            if (value)
                return result_type{fn(value.value())};
            return std::nullopt;
        }
        Map& operator++() {
            ++it;
            return *this;
        }
        pointer operator->() {
            return it.operator->();
        }
    private:
        Iter it;
        Fn fn;
    };
} // namespace rust
