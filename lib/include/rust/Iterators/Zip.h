#pragma once
#include "impl.h"
namespace rust
{
    template <typename Iter1, typename Iter2>
        RSINCPP_REQUIRES((iterator<Iter1> && iterator<Iter2>))
    struct Zip {
    public:
        using iterator_category = typename Iter1::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<typename Iter1::value_type, typename Iter2::value_type>;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Zip<Iter1, Iter2>;
        using output_type = std::optional<value_type>;
        Zip(Iter1 it1, Iter2 it2): itfirst(it1), itsecond(it2)
        {}
        Zip(const Zip& other) = default;
        Zip(Zip&& other) = default; 
        output_type operator*() {
            return is_some() ? std::optional{std::pair{(*itfirst).value(), (*itsecond).value()}} : std::nullopt;
        }
        self_type& operator++() {
            if (is_some())
            {
                ++itfirst;
                ++itsecond;
            }
            return *this;
        }

        inline bool is_some() {
            return itfirst.is_some() && itsecond.is_some();
        }
        inline bool is_none() {
            return !is_some();
        }
    private:
        Iter1 itfirst;
        Iter2 itsecond;
    };
    template <typename Iter, typename NewIter>
        RSINCPP_REQUIRES((iterator<Iter> && iterator<NewIter>))
    auto operator>>(Iter&& iter, impl::Factory<9, NewIter>&& f)
    {
        return Zip<std::remove_reference_t<Iter>, std::remove_reference_t<NewIter>>(std::forward<Iter>(iter), std::forward<NewIter>(std::get<0>(f.v)));
    }
    template <class NewIter> 
    auto zip(NewIter&& iter) 
    { 
        return impl::Factory<9, NewIter>(std::forward<NewIter>(iter)); 
    }
}