#pragma once

#include <cassert>

#include <dr/basic_types.hpp>

namespace dr
{

/// Represents a non-owning view over a range of contiguous elements
template <typename T>
struct Span
{
    using Index = isize;

    constexpr Span() = default;

    constexpr Span(T* const data, Index const size) :
        data_{data},
        size_{(assert(size >= 0), size)}
    {
    }

    /// Implicit conversion to Span of const elements
    constexpr operator Span<T const>() const { return {data_, size_}; }

    /// Returns a pointer to the first element in this span
    constexpr T* data() const { return data_; }

    /// Returns the number of elements in this span
    constexpr Index size() const { return size_; }

    /// Returns the element at the given index
    constexpr T& operator[](Index const index) const
    {
        assert(index >= 0 && index < size_);
        return *(data_ + index);
    }

    /// Returns a subspan of this span
    constexpr Span<T> segment(Index const offset, Index const count) const
    {
        assert(offset >= 0 && count >= 0 && offset + count <= size_);
        return {data_ + offset, count};
    }

    /// Returns a subspan of this span
    constexpr Span<T> range(Index const from, Index const to) const
    {
        assert(from >= 0 && from <= to && to <= size_);
        return {data_ + from, to - from};
    }

    /// Returns a subspan from the front of this span
    constexpr Span<T> front(Index const count) const
    {
        assert(count >= 0 && count <= size_);
        return {data_, count};
    }

    /// Returns a subspan from the back of this span
    constexpr Span<T> back(Index const count) const
    {
        assert(count >= 0 && count <= size_);
        return {data_ + (size_ - count), count};
    }

    /// Returns a subspan of this span
    constexpr Span<T> trim(Index const num_front, Index const num_back) const
    {
        assert(num_front >= 0 && num_back >= 0 && num_front + num_back <= size_);
        return {data_ + num_front, size_ - (num_front + num_back)};
    }

    /// Explicit conversion to a span of const elements
    constexpr Span<T const> as_const() const { return {data_, size_}; }

    /// True if this span refers to a valid memory address
    constexpr bool is_valid() const { return data_ != nullptr; }
    constexpr explicit operator bool() const { return is_valid(); }

  private:
    T* data_{};
    Index size_{};
};

/*
    Range-based for loop support
*/

template <typename T>
T* begin(Span<T> const& span) { return span.data(); }

template <typename T>
T* end(Span<T> const& span) { return span.data() + span.size(); }

} // namespace dr