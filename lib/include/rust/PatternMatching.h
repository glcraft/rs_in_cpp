#ifdef __cpp_concepts
#include <concepts>
#endif
#include <functional>
#include <tuple>
#include <optional>

#define MAKE_MULTIPLE template <Patterns<input_t> NewPattern> Multiple<input_t, self_t, NewPattern> operator| (NewPattern v){return Multiple<input_t, self_t, NewPattern>(std::move(*this), std::move(v)); }
namespace rust
{
    namespace pattern
    {

        #ifdef __cpp_concepts
        template <typename T, typename ...Types>
        concept same_several = (std::same_as<T, Types> || ...);
        template <typename T, typename Input>
        concept Patterns =
            requires(T a, Input inp) {
                {a(inp)} -> std::convertible_to<bool>;
            };
        template <typename T, typename Output>
        concept FnExecute =
            requires(T fn) {
                {fn()} ->  std::convertible_to<Output>;
            };
        #endif

        

        template <typename Input, typename Output, FnExecute<Output> Fn, Patterns<Input> Pattern>
        struct Execute 
        {
            using input_t = Input;
            using output_t = Output;
            using fn_execute = Fn;
            Execute(Pattern p, fn_execute fn): p(p), fn(fn)
            {}
            std::optional<output_t> operator()(input_t inp)
            {
                if (!p(inp))
                    return {};
                return fn(inp);
            }
        private:
            Pattern p;
            fn_execute fn;
        };

        // template<typename Input, typename Output, Patterns<Input> Pattern>
        // Execute<Input, Output, Pattern> operator >>(Pattern&& p, std::function<Output(Input)>fn)
        // {
        //     return Execute(std::move(p), fn);
        // }
        
        template <typename Input, typename ...Patterns_t>
        struct Multiple {
            using input_t = Input;
            using self_t = Multiple<input_t, Patterns_t...>;
            Multiple(Patterns_t&&... a) : m_patterns(std::forward<Patterns_t>(a) ...)
            {}
            bool operator()(input_t a) {
                return disassembly_arms<0>(a);
            };
            MAKE_MULTIPLE
        private:
            template <size_t i, typename T>
            inline bool disassembly_arms(T a) {
                if (std::get<i>(m_patterns)(a))
                    return true;
                if constexpr(i<sizeof...(Patterns_t)-1)
                    return disassembly_arms<i+1>(a);
                else 
                    return false;
            }
            std::tuple<Patterns_t...> m_patterns;
        };
        template <typename Input>
        struct Value
        {
            using input_t = Input;
            using self_t = Value<Input>;
            Value(input_t v) : value(v)
            {}
            bool operator()(input_t a) {
                return a == value;
            };
            MAKE_MULTIPLE

            template<typename Output, FnExecute<Output> Fn>
            Execute<input_t, Output, Fn, self_t> operator >>(Fn fn)
            {
                return Execute(std::move(*this), fn);
            }
            // template<typename Fn>
            // Execute<input_t, Fn, self_t> execute(Fn fn)
            // {
            //     return Execute(std::move(*this), fn);
            // }

            
        private:
            input_t value;
        };
        template <typename Input>
        struct Range
        {
            using input_t = Input;
            using self_t = Range<Input>;
            Range(input_t from, input_t to_exc) : value(from, to_exc)
            {}
            bool operator()(input_t a) {
                return a >= value.first && a < value.second;
            };
            MAKE_MULTIPLE

            
        private:
            std::pair<input_t, input_t> value;
        };
    } // namespace pattern
    
    
} // namespace rust
