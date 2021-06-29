#pragma once
#include "impl.h"
namespace rust
{
    template <typename Iter>
        RSINCPP_REQUIRES((iterator<Iter>))
    struct Intersperse {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = Intersperse<Iter>;
        using iterator_type = Iter;
        using output_type = std::optional<reference>;

        Intersperse(iterator_type it, value_type&& separator): it(it), separator(std::forward<value_type>(separator)), which(false)
        {}
        output_type operator*() {
            if (!which)
                return it.operator*();
            else if (it.is_some())
                return std::ref(separator);
            else
                return output_type{};
        }
        self_type& operator++() {
            which = !which;
            if (which && it.is_some())
                ++it;
            return *this;
        }

        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        value_type separator;
        bool which;
    };
    template <typename Iter, typename T>
    auto operator>>(Iter&& iter, impl::Factory<5, T>&& f)
    {
        return Intersperse<std::remove_reference_t<Iter>>(std::forward<Iter>(iter), std::forward<typename Iter::value_type>(std::get<0>(f.v))); 
    }
    template <typename T>
    auto intersperse(T separator) 
    { 
        return impl::Factory<5, T>(separator);
    } 
}