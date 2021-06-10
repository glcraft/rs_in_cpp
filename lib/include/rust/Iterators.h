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
} \
auto step_by(size_t step) \
{ \
    return StepBy<self_type>(*this, step); \
}
#define IMPL_FUNCS \
inline bool is_some() { \
    return it.is_some(); \
} \
inline bool is_none() { \
    return it.is_none(); \
} \


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
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = typename T::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = ForwardIterator<T>;
        using iterator_type = T;
        using output_type = std::optional<reference>;

        ForwardIterator(iterator_type from, iterator_type to) : it(from), end(to)
        {}
        template<container Cont>
        ForwardIterator(Cont& cont) : it(cont.begin()), end(cont.end())
        {}

        ForwardIterator& operator++() {
            ++it;
            return *this;
        }
        ForwardIterator operator++(int) {
            std::iterator_traits<self_type> t;
            auto tmp = *this;
            this->operator++();
            return tmp;
        }
        output_type operator*() {
            if (it != end)
                return std::ref(*it);
            return std::nullopt;
        }
        pointer operator->() {
            return it.operator->();
        }
        inline bool is_some() {
            return it!=end;
        }
        inline bool is_none() {
            return !is_some();
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
        IMPL_FUNCS
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
        IMPL_FUNCS
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
        IMPL_FUNCS
    private:
        Iter it;
        size_t count;
    };
    template <typename Iter>
    struct StepBy {
        using iterator_category = typename Iter::iterator_category;
        using self_type = StepBy<Iter>;
        using iterator_type = Iter;
        using value_type = typename Iter::value_type;
        using output_type = typename Iter::output_type;
        using pointer = value_type*;

        StepBy(iterator_type it, size_t step): it(it), step(step)
        {}
        output_type operator*() {
            return it.operator*();
        }
        self_type& operator++() {
            auto count = step;
            while(count--!=0 && it.is_some())
                ++it;
            return *this;
        }
        pointer operator->() {
            return it.operator->();
        }

        FORWARD_FUNCS
        IMPL_FUNCS
    private:
        Iter it;
        size_t step;
    };
} // namespace rust

#undef FORWARD_FUNCS
#undef IMPL_FUNCS