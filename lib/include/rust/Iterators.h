#pragma once
#include <iterator>

#define RSINCPP_FORWARD_FUNCS \
self_type operator++(int) { \
    auto tmp = *this; \
    this->operator++(); \
    return tmp; \
} \
template <class FnMap> \
auto map(FnMap&& fn) \
{ \
    using NewU = std::remove_reference_t<decltype(fn(this->operator*().value()))>; \
    return Map<self_type, NewU, FnMap>(*this, std::forward<FnMap>(fn)); \
} \
template <class PredFilter> \
auto filter(PredFilter&& pred) \
{ \
    return Filter<self_type, PredFilter>(*this, std::forward<PredFilter>(pred)); \
} \
auto enumerate() \
{ \
    return Enumerate<self_type>(*this); \
} \
auto step_by(size_t step) \
{ \
    return StepBy<self_type>(*this, step); \
} \
template <class NewIter> \
auto chain(NewIter iter) \
{ \
    return Chain<self_type, NewIter>(*this, iter); \
}
#define RSINCPP_IMPL_FUNCS \
inline bool is_some() { \
    return it.is_some(); \
} \
inline bool is_none() { \
    return !is_some(); \
} \


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
#   define RSINCPP_REQUIRES(c) requires c
#else
#   define RSINCPP_REQUIRES(c) 
#endif
    template <typename T>
        RSINCPP_REQUIRES(std::forward_iterator<T>)
    struct ForwardIterator;
    template <typename Iter, typename U, class Fn>
    struct Map;
    template <typename Iter, class Pred>
    struct Filter;
    template <typename Iter>
    struct Enumerate;
    template <typename Iter>
    struct StepBy;
    template <typename Iter1, typename Iter2>
    struct Chain;

    template <typename T>
        RSINCPP_REQUIRES(std::forward_iterator<T>)
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
        template<class Cont>
            RSINCPP_REQUIRES(container<Cont>)
        ForwardIterator(Cont& cont) : it(cont.begin()), end(cont.end())
        {}

        ForwardIterator& operator++() {
            ++it;
            return *this;
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

        RSINCPP_FORWARD_FUNCS
    protected: 
        T it, end;
    };
    template<class Cont>
        RSINCPP_REQUIRES(container<Cont>)
    ForwardIterator(Cont) -> ForwardIterator<typename Cont::iterator>;
    
    template<class Cont>
        RSINCPP_REQUIRES(container<Cont> && std::forward_iterator<typename Cont::iterator>)
    ForwardIterator<typename Cont::iterator> iter(Cont& cont)
    {
        return ForwardIterator(cont);   
    }
    template <typename Iter, typename U, class Fn>
    struct Map {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = U;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Map<Iter, U, Fn>;
        using iterator_type = Iter;
        using output_type = std::optional<U>;

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

        RSINCPP_FORWARD_FUNCS
        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        Fn fn;
    };
    template <typename Iter, class Pred>
    struct Filter {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Filter<Iter, Pred>;
        using iterator_type = Iter;
        using output_type = typename Iter::output_type;

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

        RSINCPP_FORWARD_FUNCS
        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        Pred pred;
    };
    template <typename Iter>
    struct Enumerate {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Enumerate<Iter>;
        using iterator_type = Iter;
        using output_type = std::optional<std::pair<size_t, value_type>>;

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

        RSINCPP_FORWARD_FUNCS
        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        size_t count;
    };
    template <typename Iter>
    struct StepBy {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = StepBy<Iter>;
        using iterator_type = Iter;
        using output_type = typename Iter::output_type;

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

        RSINCPP_FORWARD_FUNCS
        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        size_t step;
    };
    template <typename Iter1, typename Iter2>
    struct Chain {
    public:
        static_assert(std::is_same_v<typename Iter1::output_type, typename Iter2::output_type>, "Chain: the 2 iterators must returns the same type of value.");
        using iterator_category = typename Iter1::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter1::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Chain<Iter1, Iter2>;
        using output_type = typename Iter1::output_type;
        Chain(Iter1 it1, Iter2 it2): itfirst(it1), itsecond(it2), current(false)
        {}
        Chain(const Chain& other) = default;
        Chain(Chain&& other) = default;
        output_type operator*() {
            if (!current)
                return *itfirst;
            else
                return *itsecond;
        }
        self_type& operator++() {
            if (!current)
            {
                ++itfirst;
                if (itfirst.is_none())
                    current=true;
            }
            else
                ++itsecond;
            return *this;
        }

        RSINCPP_FORWARD_FUNCS
        inline bool is_some() {
            return itfirst.is_some() || itsecond.is_some();
        }
        inline bool is_none() {
            return !is_some();
        }
    private:
        Iter1 itfirst;
        Iter2 itsecond;
        bool current;
    };
} // namespace rust

#undef RSINCPP_FORWARD_FUNCS
#undef RSINCPP_IMPL_FUNCS
#undef RSINCPP_REQUIRES