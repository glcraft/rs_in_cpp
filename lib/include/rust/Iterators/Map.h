#pragma once
#include "impl.h"
namespace rust
{
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

        RSINCPP_IMPL_FUNCS
    private:
        Iter it;
        FnMap fn;
    };
    template <typename Iter, typename FnMap>
    auto operator>>(Iter&& iter, impl::Factory<0, FnMap>&& f)
    {
        using NewU = std::remove_reference_t<decltype(std::get<0>(f.v)((*iter).value()))>;
        return Map<std::remove_reference_t<Iter>, NewU, FnMap>(std::forward<Iter>(iter), std::forward<FnMap>(std::get<0>(f.v)));
    }
    template <class FnMap>
    auto map(FnMap&& fn)
    {
        return impl::Factory<0, FnMap>(std::forward<FnMap>(fn));
    }
}