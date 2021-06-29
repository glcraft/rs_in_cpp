#pragma once
#include "impl.h"
namespace rust
{
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
    template <typename Iter, typename NewIter>
        RSINCPP_REQUIRES((iterator<Iter> && iterator<NewIter> && std::same_as<typename Iter::value_type, typename NewIter::value_type>))
    auto operator>>(Iter&& iter, impl::Factory<4, NewIter>&& f)
    {
        return Chain<std::remove_reference_t<Iter>, std::remove_reference_t<NewIter>>(std::forward<Iter>(iter), std::forward<NewIter>(std::get<0>(f.v)));
    }
    template <class NewIter> 
    auto chain(NewIter&& iter) 
    { 
        return impl::Factory<4, NewIter>(std::forward<NewIter>(iter)); 
    }
}