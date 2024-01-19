#pragma once

#include <cstdlib> /* std::size_t, std::abs overloads */
#include <climits> /* CHAR_BIT */

#include "utility.tcc"

/**
 * @brief       Simple bit manipulation library for handy operations.
 *
 * @note        C++17 introduces this hacky syntax for nested namespaces.
 *              - https://stackoverflow.com/a/28022457
 *              - https://stackoverflow.com/a/61113363
 */
namespace crim::bit {
    static constexpr size_t bits_in_byte = CHAR_BIT;

    template<class T> 
    constexpr size_t size(const T &value);
    
    template<class T>
    constexpr size_t size();

    template<typename IntT> 
    size_t length(IntT value);
    
    template<typename IntT>
    size_t next_power(IntT value);
};


/** 
 * @brief   Number of bits that `value` takes up in total.
 */
template<class T> 
constexpr size_t crim::bit::size(const T &value)
{
    return sizeof(value) * bits_in_byte;
}

/**
 * @brief   Number of bits that `T` takes up in total with no parameter needed.
 */
template<class T>
constexpr size_t crim::bit::size()
{
    return sizeof(T) * bits_in_byte;
}

/**
 * @brief   Get the minimum number of bits needed to represent `value`.
 * 
 * @note    Bitwise operations only work for integer types.
 *          Signed negatives are ok but we will get their absolute value, since
 *          bitwise operations on negatives are undefined behaviour.
 */
template<typename IntT> 
size_t crim::bit::length(IntT value)
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
template<typename IntT>
size_t crim::bit::next_power(IntT value)
{
    ensure_integral_and_positive(value);
    // Any number raised to 0 is 1.
    if (value == 0) {
        return 1;
    }
    // Use bit length of  value - 1 so powers of 2 have the correct lengths.
    size_t power{length(value - 1)};
    // We can approximate exponentiation using bit shift left.
    return 1 << power;
}

