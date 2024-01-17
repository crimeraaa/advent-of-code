#pragma once

#include <cstdlib> /* std::size_t, std::abs overloads */
#include <climits> /* CHAR_BIT */
#include <type_traits> /* std::is_integral, std::is_signed, std::is_unsigned */

/**
 * @brief       Simple functions for getting information about numerical types.
 *
 * @note        C++17 introduces this hacky syntax for nested namespaces.
 *              - https://stackoverflow.com/a/28022457
 *              - https://stackoverflow.com/a/61113363
 */
namespace crim::digits {
    static constexpr size_t bits_in_byte = CHAR_BIT;

    template<class T> 
    constexpr void ensure_integral(T value = T());
    
    template<class T>
    constexpr void ensure_positive(T &value);
    
    template<class T>
    constexpr void ensure_integral_and_positive(T &value);

    template<class T> 
    constexpr size_t bit_size(T value = T());

    template<class IntT> 
    size_t bit_length(IntT value);
    
    template<class IntT>
    size_t bit_next_power(IntT value);
    
    template<class IntT>
    size_t count_digits(IntT value, int base = 10);
};

// If value is a reference, we can't use decltype(value) 
// This is because `std::is_integral` fails with reference types.
template<class T> 
constexpr void crim::digits::ensure_integral(T value)
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
constexpr void crim::digits::ensure_positive(T &value)
{
    if constexpr(std::is_signed<T>::value) {
        value = std::abs(value); 
    }
}

template<class T>
constexpr void crim::digits::ensure_integral_and_positive(T &value)
{
    ensure_integral(value);
    ensure_positive(value);
}

/** 
 * @brief   Number of bits that the given type `T` takes up.
 *
 * @note    You can use explicit template instantiation e.g. `bit_size<int>()`
 *          or pass a typed parameter e.g. `int x; bit_size(x);`. 
 */
template<class T> 
constexpr size_t crim::digits::bit_size(T value)
{
    return sizeof(value) * bits_in_byte;
}

/**
 * @brief   Get the minimum number of bits needed to represent `value`.
 * 
 * @note    Bitwise operations only work for integer types.
 *          Signed negatives are ok but we will get their absolute value, since
 *          bitwise operations on negatives are undefined behaviour.
 */
template<class IntT> 
size_t crim::digits::bit_length(IntT value)
{
    ensure_integral_and_positive(value);
    // `0b1101 (13)`->`0b0110 (6)`->`0b0011 (3)`->`0b0001 (1)`->`0b0000 (0)`
    size_t bit_count{0};
    while (value > 0) {
        value >>= 1;
        bit_count++;
    }
    return bit_count;
}

/**
 * @brief   Get the nearest power 2 that is greater than or equal to `value`.
 *
 * @note    Bitwise operations only work for integer types.
 *          Signed negatives are ok but we will get their absolute value, since
 *          bitwise operations on negatives are undefined behaviour.
 */
template<class IntT>
size_t crim::digits::bit_next_power(IntT value)
{
    ensure_integral_and_positive(value);
    // Any number raised to 0 is 1.
    if (value == 0) {
        return 1;
    }
    // Use bit length of  value - 1 so powers of 2 have the correct lengths.
    size_t power{bit_length(value - 1)};
    // We can approximate exponentiation using bit shift left.
    return 1 << power;
}

template<class IntT>
size_t crim::digits::count_digits(IntT value, int base)
{
    ensure_integral_and_positive(value);
    size_t count{0};
    while (value > 0) {
        value /= base;
        count++;
    }
    return count;
}
