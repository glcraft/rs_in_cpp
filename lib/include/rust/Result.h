#include <variant>
#include <functional>
namespace rust
{
    template <class Ok_T, class Err_T>
    class Result
    {
    public:
        using ok_t = Ok_T;
        using error_t = Err_T;
        Result(ok_t&& ok_result) : m_result(std::forward<ok_t>(ok_result))
        {}
        Result(error_t&& err_result) : m_result(std::forward<error_t>(err_result))
        {}
        const Result& ok(std::function<void(const ok_t&)> fn_ok) const
        {
            if (is_ok())
                fn_ok(std::get<ok_t>(m_result));
            return *this;
        }
        const Result& error(std::function<void(const error_t&)> fn_err) const
        {
            if (is_err())
                fn_err(std::get<error_t>(m_result));
            return *this;
        }
        inline bool is_ok() const noexcept {
            return m_result.index() == 0;
        }
        inline bool is_err() const noexcept {
            return !is_ok();
        }
    private:
        std::variant<ok_t, error_t> m_result;
    };
} // namespace rust
