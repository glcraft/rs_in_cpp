#pragma once
#include "impl.h"
namespace rust 
{
    
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
    template <typename Iter, typename PredFilter>
    auto operator>>(Iter&& iter, impl::Factory<1, PredFilter>&& f)
    {
        return Filter<std::remove_reference_t<Iter>, PredFilter>(std::forward<Iter>(iter), std::forward<PredFilter>(std::get<0>(f.v))); 
    }
    template <class PredFilter> 
    auto filter(PredFilter&& pred) 
    { 
        return impl::Factory<1, PredFilter>(std::forward<PredFilter>(pred));
    } 
}