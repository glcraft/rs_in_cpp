#pragma once
#include "impl.h"
namespace rust
{
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

        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        size_t count;
    };
    template <typename Iter>
        RSINCPP_REQUIRES(iterator<std::remove_reference_t<Iter>>)
    auto operator>>(Iter&& iter, impl::Factory<2> f)
    {
        return Enumerate<std::remove_reference_t<Iter>>(std::forward<Iter>(iter));
    }
    auto enumerate() 
    { 
        return impl::Factory<2>();
    } 
}