#pragma once
#include "impl.h"
namespace rust
{
    template <typename Iter>
        RSINCPP_REQUIRES((iterator<Iter>))
    struct Take {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Take<Iter>;
        using iterator_type = Iter;
        using output_type = std::optional<reference>;

        Take(iterator_type it, size_t ntake): it(it), count(ntake>0 ? std::optional{ntake} : std::nullopt)
        {}

        output_type operator*() {
            if (!count.has_value())
                return std::nullopt;
            return *it;
        }
        self_type& operator++() {
            if (take() && it.is_some())
                ++it;
            return *this;
        }

        RSINCPP_IMPL_FUNCS
    private:
        bool take()
        {
            if (count.has_value())
            {
                if (--count.value()==0)
                    count = std::nullopt;
                return true;
            }
            return false;
        }
        Iter it;
        std::optional<size_t> count;
    };
    template <typename Iter>
        RSINCPP_REQUIRES(iterator<std::remove_reference_t<Iter>>)
    auto operator>>(Iter&& iter, impl::Factory<8, size_t> f)
    {
        return Take<std::remove_reference_t<Iter>>(std::forward<Iter>(iter), std::get<0>(f.v));
    }
    auto take(size_t ntake) 
    { 
        return impl::Factory<8, size_t>(ntake);
    } 
}