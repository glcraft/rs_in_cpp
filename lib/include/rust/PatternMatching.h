#ifdef __cpp_concepts
#include <concepts>
#endif
#include <functional>
#include <tuple>
namespace rust
{
    namespace pattern
    {
        template <typename T>
        struct Value;
        template <typename T>
        struct Ranges;
        // template <typename ...T>
        // struct Multiple;

        #ifdef __cpp_concepts
        template <typename T, typename ...Types>
        concept same_several = (std::same_as<T, Types> || ...);
        template <typename T, typename Input>
        concept Patterns = //same_several<T, Value<Input>, Ranges<Input>>;
            requires(T a, Input inp) {
                {a(inp)} -> std::convertible_to<bool>;
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
        
        template <typename Input, typename ...Patterns_t>
        struct Multiple {
            using input_t = Input;
            using me_t = Multiple<input_t, Patterns_t...>;
            Multiple(Patterns_t&&... a) : m_arms(std::forward<Patterns_t>(a) ...)
            {}
            bool operator()(input_t a) {
                return disassembly_arms<0>(a);
            };
            template <class NewPattern>
                requires Patterns<NewPattern, input_t>
            Multiple<input_t, me_t, NewPattern> operator |(NewPattern v)
            {
                return Multiple<input_t, me_t, NewPattern>{std::move(*this), std::move(v)};
            }
        private:
            template <size_t i, typename T>
            inline bool disassembly_arms(T a) {
                if (std::get<i>(m_arms)(a))
                    return true;
                if constexpr(i<sizeof...(Patterns_t)-1)
                    return disassembly_arms<i+1>(a);
                else 
                    return false;
            }
            std::tuple<Patterns_t...> m_arms;
        };
        template <typename Input>
        struct Value
        {
            using input_t = Input;
            Value(input_t v) : value(v)
            {}
            bool operator()(input_t a) {
                return a == value;
            };
            template <class NewPattern>
                requires Patterns<NewPattern, input_t>
            Multiple<input_t, Value<input_t>, NewPattern> operator |(NewPattern v)
            {
                return Multiple<input_t, Value<input_t>, NewPattern>(std::move(*this), std::move(v));
            }

            input_t value;
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
