#pragma once
#include <iterator>
#include <tuple>

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
    template <typename T>
        RSINCPP_REQUIRES(std::forward_iterator<T>)
    struct ForwardIterator;
    template <typename Iter, typename Output, class FnMap>
        RSINCPP_REQUIRES((iterator<Iter> && map_function<FnMap, typename Iter::value_type, Output>))
    struct Map;
    template <typename Iter, class Pred>
        RSINCPP_REQUIRES((iterator<Iter> && predicate_function<Pred, typename Iter::value_type>))
    struct Filter;
    template <typename Iter>
        RSINCPP_REQUIRES((iterator<Iter>))
    struct Enumerate;
    template <typename Iter>
        RSINCPP_REQUIRES((iterator<Iter>))
    struct StepBy;
    template <typename Iter1, typename Iter2>
        RSINCPP_REQUIRES((iterator<Iter1> && iterator<Iter2> && std::same_as<typename Iter1::value_type, typename Iter2::value_type>))
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
    template <typename Iter, typename Output, class FnMap>
        RSINCPP_REQUIRES((iterator<Iter> && map_function<FnMap, typename Iter::value_type, Output>))
    struct Map {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = Output;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Map<Iter, Output, FnMap>;
        using iterator_type = Iter;
        using output_type = std::optional<Output>;

        Map(iterator_type it, FnMap&& fn): it(it), fn(std::forward<FnMap>(fn))
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

        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        FnMap fn;
    };
    template <typename Iter, class Pred>
        RSINCPP_REQUIRES((iterator<Iter> && predicate_function<Pred, typename Iter::value_type>))
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

        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        Pred pred;
    };
    template <typename Iter>
        RSINCPP_REQUIRES((iterator<Iter>))
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

        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        size_t count;
    };
    template <typename Iter>
        RSINCPP_REQUIRES((iterator<Iter>))
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

        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        size_t step;
    };
    template <typename Iter1, typename Iter2>
        RSINCPP_REQUIRES((iterator<Iter1> && iterator<Iter2> && std::same_as<typename Iter1::value_type, typename Iter2::value_type>))
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
    template<uint32_t IterType, typename ...Types>
    struct Factory {
        std::tuple<Types...> v;
    };
    template <typename Iter, typename FnMap>
    auto operator>>(Iter&& iter, Factory<0, FnMap>&& f)
    {
        using NewU = std::remove_reference_t<decltype(std::get<0>(f.v)((*iter).value()))>;
        return Map<std::remove_reference_t<Iter>, NewU, FnMap>(std::forward<Iter>(iter), std::forward<FnMap>(std::get<0>(f.v)));
    }
    template <typename Iter, typename PredFilter>
    auto operator>>(Iter&& iter, Factory<1, PredFilter>&& f)
    {
        return Filter<std::remove_reference_t<Iter>, PredFilter>(std::forward<Iter>(iter), std::forward<PredFilter>(std::get<0>(f.v))); 
    }
    template <typename Iter>
        RSINCPP_REQUIRES(iterator<std::remove_reference_t<Iter>>)
    auto operator>>(Iter&& iter, Factory<2> f)
    {
        return Enumerate<std::remove_reference_t<Iter>>(std::forward<Iter>(iter));
    }
    template <typename Iter>
    auto operator>>(Iter&& iter, Factory<3, size_t>&& f)
    {
        return StepBy<std::remove_reference_t<Iter>>(std::forward<Iter>(iter), std::get<0>(f.v)); 
    }
    template <typename Iter, typename NewIter>
        RSINCPP_REQUIRES((iterator<Iter> && iterator<NewIter> && std::same_as<typename Iter::value_type, typename NewIter::value_type>))
    auto operator>>(Iter&& iter, Factory<4, NewIter>&& f)
    {
        return Chain<std::remove_reference_t<Iter>, std::remove_reference_t<NewIter>>(std::forward<Iter>(iter), std::forward<NewIter>(std::get<0>(f.v)));
    }

    template <class FnMap>
    auto map(FnMap&& fn)
    {
        return Factory<0, FnMap>(std::forward<FnMap>(fn));
    }
    template <class PredFilter> 
    auto filter(PredFilter&& pred) 
    { 
        return Factory<1, PredFilter>(std::forward<PredFilter>(pred));
    } 
    auto enumerate() 
    { 
        return Factory<2>();
    } 
    auto step_by(size_t step) 
    { 
        return Factory<3, size_t>(step);
    } 
    template <class NewIter> 
    auto chain(NewIter&& iter) 
    { 
        return Factory<4, NewIter>(std::forward<NewIter>(iter)); 
    }
} // namespace rust

#undef RSINCPP_IMPL_FUNCS
#undef RSINCPP_REQUIRES