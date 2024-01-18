#pragma once

#include "type_traits.tcc"

namespace crim {
    template<class T>
    constexpr remove_reference_t<T> &&rvalue_cast(T &&value) noexcept;
};

/** 
 * Basically `std::move`.
 */
template<class T>
constexpr crim::remove_reference_t<T> &&crim::rvalue_cast(T &&value) noexcept
{
    // Just in case `T` is in itself a reference type of some sort.
    using raw_type = typename remove_reference<T>::type;
    return static_cast<raw_type &&>(value);
}
