#pragma once

/**
 * @brief       `#pragma region` isn't recognized by GCC I think. 
 *              So ignore the pragma as it's not used at compile-time.
 *              - https://stackoverflow.com/a/22539400
 * 
 * @warning     Be sure to pop back this pragma though, else other compilation 
 *              units won't be checked for unknown pragmas!
 *              
 * @note        Seems this is bugged and GCC doesn't respect this properly.
 *              - https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431
 *              - https://stackoverflow.com/a/55078975
 */
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

// #pragma region crim_remove_reference
/**
 * BEGIN: remove_reference -*---------------------------------------------------
 */
// https://en.cppreference.com/w/cpp/types/remove_reference

namespace crim {
    template<class T>
    struct remove_reference;
};

/**
 * @brief   "Base" version, although rather pointless on its own for immediate types.
 *          This is meant more for completeness so that it just works.
 */
template<class T>
struct crim::remove_reference {
    using type = T;
};

/**
 * @brief   Specialization to extract the raw type from lvalue references.
 */
template<class T>
struct crim::remove_reference<T&> {
    using type = T;
};

/**
 * @brief   Specialization to extract the raw type from rvalue references.
 */
template<class T>
struct crim::remove_reference<T&&> {
    using type = T;
};

/**
 * END: remove_reference  -*----------------------------------------------------
 */
// #pragma endregion crim_remove_reference

namespace crim {
    /**
     * @brief   Templated helper type which is somewhat nicer to use.
     *          Functinally no difference from `remove_reference<T>::type`.
     *          - https://en.cppreference.com/w/cpp/types/remove_reference
     */
    template<class T>
    using remove_reference_t = typename remove_reference<T>::type;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "impl/char_traits.tcc"
