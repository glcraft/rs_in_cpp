#pragma once
#include "impl.h"
namespace rust
{
    template <typename T>
        RSINCPP_REQUIRES(std::forward_iterator<T>)
    struct ForwardIterator : std::forward_iterator_tag
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = typename T::value_type;
        using pointer = value_type*;
        using reference = std::reference_wrapper<value_type>;
        using self_type = ForwardIterator<T>;
        using iterator_type = T;
        using output_type = std::optional<reference>;

        ForwardIterator(iterator_type from, iterator_type to) : it(from), end(to)
        {}
        template<class Cont>
            RSINCPP_REQUIRES(container<Cont>)
        ForwardIterator(Cont& cont) : it(cont.begin()), end(cont.end())
        {}

        ForwardIterator& operator++() {
            ++it;
            return *this;
        }
        output_type operator*() {
            if (it != end)
                return std::ref(*it);
            return std::nullopt;
        }
        inline bool is_some() {
            return it!=end;
        }
        inline bool is_none() {
            return !is_some();
        }

        protected: 
        T it, end;
    };
    template<class Cont>
        RSINCPP_REQUIRES(container<Cont>)
    ForwardIterator(Cont) -> ForwardIterator<typename Cont::iterator>;
    
    template<class Cont>
        RSINCPP_REQUIRES(container<Cont> && std::forward_iterator<typename Cont::iterator>)
    ForwardIterator<typename Cont::iterator> iter(Cont& cont)
    {
        return ForwardIterator(cont);   
    }
} // namespace rust
