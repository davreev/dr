#pragma once

#include <cassert>

#include <type_traits>
#include <utility>

namespace dr
{

template <typename Error>
struct ErrorResult
{
    static_assert(std::is_integral_v<Error> || std::is_enum_v<Error>);

    Error error{};
};

// Deduction guide
template <typename Error>
ErrorResult(Error&&) -> ErrorResult<Error>;

template <typename Value, typename Error, Error no_error = Error{}>
struct Result
{
    static_assert(std::is_integral_v<Error> || std::is_enum_v<Error>);

    template <typename Value_, std::enable_if_t<std::is_convertible_v<Value_, Value>>* = nullptr>
    constexpr Result(Value_&& value) : value_{std::forward<Value_>(value)}, error_{no_error}
    {
    }

    constexpr Result(ErrorResult<Error>&& result) : error_{result.error} {}

    constexpr bool has_value() const { return error_ == no_error; }
    constexpr explicit operator bool() const { return has_value(); }

    constexpr Value& value()
    {
        assert(has_value());
        return value_;
    }

    constexpr Value const& value() const
    {
        return const_cast<Result<Value, Error>*>(this)->value();
    }

    constexpr Error error() const { return error_; }

  private:
    Value value_{};
    Error error_{};
};

template <typename Value>
struct Maybe
{
    template <typename Value_, std::enable_if_t<std::is_convertible_v<Value_, Value>>* = nullptr>
    constexpr Maybe(Value_&& value) : value_{std::forward<Value_>(value)}, has_value_{true}
    {
    }

    constexpr Maybe() = default;

    constexpr bool has_value() const { return has_value_; }
    constexpr explicit operator bool() const { return has_value_; }

    constexpr Value& value()
    {
        assert(has_value());
        return value_;
    }

    constexpr Value const& value() const { return const_cast<Maybe<Value>*>(this)->value(); }

  private:
    Value value_{};
    bool has_value_{};
};

} // namespace dr