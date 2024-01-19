#pragma once

/**
 * Ironic, but I believe most of these are compiler builtins so what can you do? 
 */
#include <type_traits> /* std::is_* family */

#include "type_traits.tcc"

namespace crim {
    template<class T> 
    constexpr void ensure_integral(T value = T());
    
    template<class T>
    constexpr void ensure_positive(T &value);
    
    template<class T>
    constexpr void ensure_integral_and_positive(T &value);

    template<class T>
    constexpr remove_reference_t<T> &&rvalue_cast(T &&value) noexcept;

    template<class T>
    constexpr size_t count_bytes(size_t n_count);

    template<typename IntT>
    size_t count_digits(IntT value, int base = 10);
};

// If value is a reference, we can't use decltype(value) 
// This is because `std::is_integral` fails with reference types.
template<class T> 
constexpr void crim::ensure_integral(T value)
{
    (void)value;
    static_assert(std::is_integral<T>::value, "bruh");
}

/**
 * `std::abs` on unsigneds can mess up, causes program to be ill-formed
 *
 * `double` can only represent -2^53--2^53, `long long` is -2^63--2^63, 
 * might error if using <cmath> `std::abs`, so use <cstdlib> `std::abs`.
 *
 * - https://en.cppreference.com/w/cpp/numeric/math/abs
 */
template<class T>
constexpr void crim::ensure_positive(T &value)
{
    if constexpr(std::is_signed<T>::value) {
        value = std::abs(value); 
    }
}

template<class T>
constexpr void crim::ensure_integral_and_positive(T &value)
{
    ensure_integral(value);
    ensure_positive(value);
}

/** 
 * @brief   Basically `std::move`. Useful for forcing move constructor/assign.
 */
template<class T>
constexpr crim::remove_reference_t<T> &&crim::rvalue_cast(T &&value) noexcept
{
    // Just in case `T` is in itself a reference type of some sort.
    return static_cast<typename remove_reference<T>::type &&>(value);
}

/**
 * @brief   Quick and dirty wrapper around `sizeof(T) * n_count`,
 *          for all your malloc'ing needs.
 */
template<class T>
constexpr size_t crim::count_bytes(size_t n_count)
{
    return sizeof(T) * n_count;
}

/**
 * @brief   Get the number of digits needed to represent `value` in `base`.
 *
 * @note    For now this only works with integers.
 */
template<typename IntT>
size_t crim::count_digits(IntT value, int base)
{
    ensure_integral_and_positive(value);
    size_t count{0};
    while (value > 0) {
        value /= base;
        count++;
    }
    return count;
}
