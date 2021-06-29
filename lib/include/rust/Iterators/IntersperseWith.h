#pragma once
#include "impl.h"
namespace rust
{
    template <typename Iter, typename FnSeparator>
        RSINCPP_REQUIRES((iterator<Iter> && requires(FnSeparator fn) {
            {fn()} -> std::convertible_to<typename Iter::value_type>;
        }))
    struct IntersperseWith {
        using iterator_category = typename Iter::iterator_category;
        using difference_type = std::ptrdiff_t;
        using value_type = typename Iter::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = IntersperseWith<Iter, FnSeparator>;
        using iterator_type = Iter;
        using output_type = std::optional<value_type>;

        IntersperseWith(iterator_type it, FnSeparator&& fn_separator): it(it), fn_separator(std::forward<FnSeparator>(fn_separator)), which(false)
        {}
        output_type operator*() {
            if (!which)
                return it.operator*();
            else if (it.is_some())
                return fn_separator();
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
        FnSeparator fn_separator;
        bool which;
    };
    template <typename Iter, typename FnSeparator>
    auto operator>>(Iter&& iter, impl::Factory<6, FnSeparator>&& f)
    {
        return IntersperseWith<std::remove_reference_t<Iter>, FnSeparator>(std::forward<Iter>(iter), std::forward<FnSeparator>(std::get<0>(f.v))); 
    }
    template <typename FnSeparator>
    auto intersperse_with(FnSeparator separator) 
    { 
        return impl::Factory<6, FnSeparator>(separator);
    } 
}