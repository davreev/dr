#pragma once

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

    Value value{};
    Error error{};

    template <typename Value_>
    constexpr Result(Value_&& value) : value{std::forward<Value_>(value)}, error{no_error}
    {
    }

    constexpr Result(ErrorResult<Error>&& result) : error{result.error} {}

    constexpr explicit operator bool() const { return error == no_error; }
};

template <typename Value>
struct Maybe
{
    Value value{};
    bool has_value{};

    template <typename Value_>
    constexpr Maybe(Value_&& value) : value{std::forward<Value_>(value)}, has_value{true}
    {
    }

    constexpr Maybe() = default;

    constexpr explicit operator bool() const { return has_value; }
};

} // namespace dr