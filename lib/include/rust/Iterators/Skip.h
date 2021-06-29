#pragma once
#include "impl.h"
namespace rust
{
    template <typename Iter>
        RSINCPP_REQUIRES((iterator<Iter>))
    struct Skip {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Skip<Iter>;
        using iterator_type = Iter;
        using output_type = std::optional<reference>;

        Skip(iterator_type it, size_t nskip): it(it), count(nskip>0 ? std::optional{nskip} : std::nullopt)
        {}

        output_type operator*() {
            skip();
            return *it;
        }
        self_type& operator++() {
            if (!skip())
                ++it;
            return *this;
        }

        RSINCPP_IMPL_FUNCS
    private:
        bool skip()
        {
            auto had_value = count.has_value();
            if (count.has_value())
            {
                for (;count.value()>0 && it.is_some();--count.value())
                    ++it;
                count = std::nullopt;
            }
            return had_value;
        }
        Iter it;
        std::optional<size_t> count;
    };
    template <typename Iter>
        RSINCPP_REQUIRES(iterator<std::remove_reference_t<Iter>>)
    auto operator>>(Iter&& iter, impl::Factory<7, size_t> f)
    {
        return Skip<std::remove_reference_t<Iter>>(std::forward<Iter>(iter), std::get<0>(f.v));
    }
    auto skip(size_t nskip) 
    { 
        return impl::Factory<7, size_t>(nskip);
    } 
}