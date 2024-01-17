#pragma once

#include "type_traits.tcc"

namespace crim {
    /**
     * @brief   Casts an arbitrary type to its rvalue counterpart. 
     *          Useful to force-invoke move-constructors and move-assignment.
     *          - https://en.cppreference.com/w/cpp/utility/move
     *  
     * @note    We use `crim::remove_reference<T>::type` to extract the raw type.
     *          This is done first in case `value` is a reference-type.
     *          Then we just force-cast it to be a `T&&` via `static_cast`.
     */
    template<class T>
    constexpr remove_reference_t<T> &&move(T &&value) noexcept
    {
        return static_cast<typename remove_reference<T>::type&&>(value);
    }
};
