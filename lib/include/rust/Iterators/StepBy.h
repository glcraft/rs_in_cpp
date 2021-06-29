#pragma once
#include "impl.h"
namespace rust
{
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
    template <typename Iter>
    auto operator>>(Iter&& iter, impl::Factory<3, size_t>&& f)
    {
        return StepBy<std::remove_reference_t<Iter>>(std::forward<Iter>(iter), std::get<0>(f.v)); 
    }
    auto step_by(size_t step) 
    { 
        return impl::Factory<3, size_t>(step);
    } 
}