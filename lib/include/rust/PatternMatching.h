#ifdef __cpp_concepts
#include <concepts>
#endif
#include <functional>
#include <tuple>
namespace rust
{
    namespace pattern
    {
        #ifdef __cpp_concepts
        template<typename T, typename U>
        concept IsEqualComparable = requires(T a, U b) {
            { a == b } -> std::convertible_to<bool>;
        };
        template<typename T, typename U>
        concept IsComparable = requires(T a, U b) {
            { a == b } -> std::convertible_to<bool>;
            { a < b } -> std::convertible_to<bool>;
            { a > b } -> std::convertible_to<bool>;
        };
        #endif
        
        // struct Base
        // {
        //     // template <typename ...Args>
        //     bool operator >>(std::function<void()> fn_ok) // Args...
        //     {
        //         fn_ok();
        //         return true;
        //     }
        // };
        template <typename ...Pattern>
        struct Multiple {
            Multiple(Pattern&&... a) : m_arms(std::forward<Pattern>(a) ...)
            {}
            template <typename T>
            bool operator()(T a) {
                return disassembly_arms<0>(a);
            };
        private:
            template <size_t i, typename T>
            inline bool disassembly_arms(T a) {
                if (std::get<i>(m_arms)(a))
                    return true;
                if constexpr(i<sizeof...(Pattern))
                    return disassembly_arms<i+1>(a);
                else 
                    return false;
            }
            std::tuple<Pattern...> m_arms;
        };
        template <typename T>
        struct Value
        {
            Value(T v) : value(v)
            {}
            bool operator()(T a) {
                return a == value;
            };
            Multiple<Value<T>, Value<T>> operator |(Value<T> v)
            {
                return Multiple<Value<T>, Value<T>>(*this, v);
            }

            T value;
        };
        template <typename Input, typename Output, typename ...Arms>
        Output match(Input& v, Arms... arms)
        {
            std::optional<Output> res;
            // ([&res] () mutable {
            //     if (arms())
            //         res = 
            // }, ...)
        }
        // template <typename Input, typename Output>
        // struct match 
        // {
        //     template <typename ...Arms>
        //     match(T& v, Arms arms...) {

        //     }
        //     Output operator();
        // };
    } // namespace pattern
    
    
} // namespace rust
