#include <variant>
#include <functional>
#include <utility>
#include <optional>
#define RUST_OK_HELPER(x,y) if (is_ok()) return x; else return y;
namespace rust
{
    namespace impl {
        template <class Ok_T, class Err_T>
        class Result
        {
        public:
            using ok_t = Ok_T;
            using const_t = Ok_T;
            using error_t = Err_T;
        protected:
            static constexpr int ok_v = 0;
            static constexpr int error_v = 1;
            template <class U>
            using ResultOk = Result<U,error_t>;
            template <class O>
            using ResultErr = Result<ok_t, O>;
        public:
            Result() = delete;
            static Result<ok_t, error_t> Ok(ok_t ok_result) noexcept {
                return Result{value_t{std::in_place_index<0>, ok_result}};
            }
            static Result<ok_t, error_t> Err(error_t err_result) noexcept {
                return Result(value_t{std::in_place_index<1>, err_result});
            }
            
            const Result& match_ok(std::function<void(const ok_t&)> fn_ok) const
            {
                if (is_ok())
                    fn_ok(std::get<ok_v>(m_result));
                return *this;
            }
            const Result& match_error(std::function<void(const error_t&)> fn_err) const
            {
                if (is_err())
                    fn_err(std::get<error_v>(m_result));
                return *this;
            }
            const Result& match_ok(std::function<void(ok_t&)> fn_ok)
            {
                if (is_ok())
                    fn_ok(std::get<ok_v>(m_result));
                return *this;
            }
            const Result& match_error(std::function<void(error_t&)> fn_err)
            {
                if (is_err())
                    fn_err(std::get<error_v>(m_result));
                return *this;
            }
            Result& match_ok(std::function<void(ok_t)> fn_ok) const
            {
                if (is_ok())
                    fn_ok(std::forward<error_t>(std::get<ok_v>(m_result)));
                return *this;
            }
            Result& match_error(std::function<void(error_t)> fn_err) const
            {
                if (is_err())
                    fn_err(std::forward<error_t>(std::get<error_v>(m_result)));
                return *this;
            }
            inline bool is_ok() const noexcept {
                return m_result.index() == 0;
            }
            inline bool is_err() const noexcept {
                return !is_ok();
            }
            std::optional<ok_t> ok() noexcept {
                RUST_OK_HELPER(std::forward<ok_t>(std::get<ok_v>(m_result)),{})
            }
            std::optional<error_t> err() noexcept {
                RUST_OK_HELPER({},std::forward<error_t>(std::get<error_v>(m_result)))
            }
            template <class U>
            ResultOk<U> map(std::function<U(ok_t)> fn_map) {
                RUST_OK_HELPER(ResultOk<U>::Ok(fn_map(std::forward<ok_t>(std::get<ok_v>(m_result)))),ResultOk<U>::Err(std::forward<error_t>(std::get<error_v>(m_result))))
            }
            template <class O>
            ResultErr<O> map_err(std::function<O(error_t)> fn_map_err) {
                RUST_OK_HELPER(ResultErr<O>::Ok(std::forward<ok_t>(std::get<ok_v>(m_result))),ResultErr<O>::Err(fn_map_err(std::forward<error_t>(std::get<error_v>(m_result)))))
            }
            template <class U>
            U map_or(U def, std::function<U(ok_t)> fn_map) {
                RUST_OK_HELPER(fn_map(std::forward<ok_t>(std::get<ok_v>(m_result))),def)
            }
            template <class U>
            U map_or_else(std::function<U(error_t)> fn_is_err, std::function<U(ok_t)> fn_is_ok) {
                RUST_OK_HELPER(fn_is_ok(std::forward<ok_t>(std::get<ok_v>(m_result))),fn_is_err(std::forward<error_t>(std::get<error_v>(m_result))))
            }
            /// and is used in C++ for "&&"
            template <class U>
            ResultOk<U> and_r(ResultOk<U> res) noexcept {
                RUST_OK_HELPER(res,ResultOk<U>::Err(std::forward<error_t>(std::get<error_v>(m_result))))
            }
            template <class U>
            ResultOk<U> and_then(std::function<ResultOk<U>(ok_t)> fn_and) {
                RUST_OK_HELPER(fn_and(std::forward<ok_t>(std::get<ok_v>(m_result))),ResultOk<U>::Err(std::forward<error_t>(std::get<error_v>(m_result))))
            }
            /// or is used in C++ for "||"
            template <class F>
            ResultErr<F> or_r(ResultErr<F> res) noexcept {
                RUST_OK_HELPER(ResultErr<F>::Ok(std::forward<ok_t>(std::get<ok_v>(m_result))),res)
            }
            template <class F>
            ResultErr<F> or_else(std::function<ResultErr<F>(error_t)> fn_or) {
                RUST_OK_HELPER(ResultErr<F>::Ok(std::forward<ok_t>(std::get<ok_v>(m_result))),fn_or(std::forward<error_t>(std::get<error_v>(m_result))))
            }
            ok_t unwrap_or(ok_t def) noexcept {
                RUST_OK_HELPER(std::forward<ok_t>(std::get<ok_v>(m_result)),def)
            }
            ok_t unwrap_or_else(std::function<ok_t(error_t)> fn_or) {
                RUST_OK_HELPER(std::forward<ok_t>(std::get<ok_v>(m_result)),fn_or(std::forward<error_t>(std::get<error_v>(m_result))))
            }
        protected:
            using value_t = std::variant<Ok_T, Err_T>;
            Result(value_t&& v) : m_result(std::forward<value_t>(v))
            {}
            std::variant<ok_t, error_t> m_result;
        };    
    }
    template <class Ok_T, class Err_T>
    class Result : public impl::Result<Ok_T, Err_T>
    {
    protected:
        using value_t = std::variant<Ok_T, Err_T>;
        using base_t = impl::Result<Ok_T, Err_T>;
        Result(value_t&& v) : base_t(std::forward<value_t>(v))
        {}
    public:
        using ok_t = Ok_T;
        using error_t = Err_T;
        Result(ok_t ok_result) : base_t::m_result(std::forward<ok_t>(ok_result))
        {}
        Result(error_t err_result) : base_t::m_result(std::forward<error_t>(err_result))
        {}
        static Result<ok_t, error_t> Ok(ok_t ok_result) noexcept {
            return Result{value_t{std::in_place_index<0>, ok_result}};
        }
        static Result<ok_t, error_t> Err(error_t err_result) noexcept {
            return Result(value_t{std::in_place_index<1>, err_result});
        }
    };
    template <class Ok_Err_T>
    class Result<Ok_Err_T, Ok_Err_T> : public impl::Result<Ok_Err_T, Ok_Err_T>
    {
    public:
        using ok_t = Ok_Err_T;
        using error_t = Ok_Err_T;
    private:
        using base_t = impl::Result<Ok_Err_T, Ok_Err_T>;
        using value_t = std::variant<ok_t, error_t>;
        Result(value_t&& v) : base_t(std::forward<value_t>(v))
        {}
    public:
        static Result<ok_t, error_t> Ok(ok_t ok_result) noexcept {
            return Result{value_t{std::in_place_index<0>, ok_result}};
        }
        static Result<ok_t, error_t> Err(error_t err_result) noexcept {
            return Result{value_t{std::in_place_index<1>, err_result}};
        }
        //Constructor are ambigous here. Use Result::Ok() or Result::Err() instead
    };
} // namespace rust
#undef RUST_OK_HELPER