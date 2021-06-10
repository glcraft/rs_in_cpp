#pragma once
#include <iterator>

#define FORWARD_FUNCS template <class Fn> \
auto map(Fn&& fn) \
{ \
    using U = std::remove_reference_t<decltype(fn(this->operator*().value()))>; \
    return Map<self_type, U, Fn>(*this, std::forward<Fn>(fn)); \
} \
template <class Pred> \
auto filter(Pred&& pred) \
{ \
    return Filter<self_type, Pred>(*this, std::forward<Pred>(pred)); \
} \
auto enumerate() \
{ \
    return Enumerate<self_type>(*this); \
}


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
        using output_type = std::optional<value_type>;
        using pointer = value_type*;

        ForwardIterator(iterator_type from, iterator_type to) : it(from), end(to)
        {}
        template<container Cont>
        ForwardIterator(Cont& cont) : it(cont.begin()), end(cont.end())
        {}

        ForwardIterator& operator++() {
            ++it;
            return *this;
        }
        output_type operator*() {
            if (it != end)
                return *it;
            return std::nullopt;
        }
        pointer operator->() {
            return it.operator->();
        }
        
        FORWARD_FUNCS
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
        using self_type = Map<Iter, U, Fn>;
        using iterator_type = Iter;
        using value_type = U;
        using output_type = std::optional<U>;
        using pointer = value_type*;

        Map(iterator_type it, Fn&& fn): it(it), fn(std::forward<Fn>(fn))
        {}
        output_type operator*() {
            auto value = *it;
            if (value)
                return output_type{fn(value.value())};
            return std::nullopt;
        }
        self_type& operator++() {
            ++it;
            return *this;
        }
        pointer operator->() {
            return it.operator->();
        }

        FORWARD_FUNCS
    private:
        Iter it;
        Fn fn;
    };
    template <typename Iter, class Pred>
    struct Filter {
        using self_type = Filter<Iter, Pred>;
        using iterator_type = Iter;
        using value_type = typename Iter::value_type;
        using output_type = typename Iter::output_type;
        using pointer = value_type*;

        Filter(iterator_type it, Pred&& pred): it(it), pred(std::forward<Pred>(pred))
        {}

        output_type operator*() {
            return it.operator*();
        }
        self_type& operator++() {
            auto value = *++it;
            while (value.has_value() && !pred(value.value())) {
                value = *++it;
            }
            return *this;
        }
        pointer operator->() {
            return it.operator->();
        }

        FORWARD_FUNCS
    private:
        Iter it;
        Pred pred;
    };
    template <typename Iter>
    struct Enumerate {
        using self_type = Enumerate<Iter>;
        using iterator_type = Iter;
        using value_type = typename Iter::value_type;
        using output_type = std::optional<std::pair<size_t, value_type>>;
        using pointer = value_type*;

        Enumerate(iterator_type it): it(it), count(0)
        {}

        output_type operator*() {
            auto value = *it;
            if (value.has_value())
                return std::pair{count, value.value()};
            else
                return std::nullopt;
        }
        self_type& operator++() {
            ++it;
            ++count;
            return *this;
        }
        pointer operator->() {
            return it.operator->();
        }

        FORWARD_FUNCS
    private:
        Iter it;
        size_t count;
    };
} // namespace rust

#undef FORWARD_FUNCS