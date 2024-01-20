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

/**
 * BEGIN: integral_constant -*--------------------------------------------------
*/

namespace crim {
    template<typename T, T t_comptime>
    struct integral_constant;
}

template<typename T, T t_comptime>
struct crim::integral_constant {
    /**
     * Use this to extract the template parameter `T` type.
     */
    using value_type = T;
    
    /**
     * Use this to extract the template instance type.
     */
    using type = integral_constant<value_type, t_comptime>;
    
    /**
     * Use this to extract the template parameter `t_comptime` value.
     */
    static constexpr value_type value = t_comptime;
    
    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

namespace crim {
    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;
    template<bool b_comptime>
    using bool_constant = integral_constant<bool, b_comptime>;
};

/**
 * END: integral_constant -*----------------------------------------------------
*/

/**
 * BEGIN: remove_cv -*----------------------------------------------------------
*/

namespace crim {
    template<typename T>
    struct remove_cv;
};

/**
 * Non-specialized. The real work is in the specializations, see below.
 */
template<typename T>
struct crim::remove_cv {
    using type = T;
};

template<typename T>
struct crim::remove_cv<const T> {
    using type = T;
};

template<typename T>
struct crim::remove_cv<volatile T> {
    using type = T;
};

template<typename T>
struct crim::remove_cv<const volatile T> {
    using type = T;
};

/**
 * END: remove_cv -*------------------------------------------------------------
*/

/**
 * BEGIN: remove_reference -*---------------------------------------------------
 */
// https://en.cppreference.com/w/cpp/types/remove_reference

namespace crim {
    template<class T>
    struct remove_reference;
};

/**
 * The real work is in the specializations, see below.
 */
template<class T>
struct crim::remove_reference {
    using type = T;
};

template<class T>
struct crim::remove_reference<T&> {
    using type = T;
};

template<class T>
struct crim::remove_reference<T&&> {
    using type = T;
};

/**
 * END: remove_reference  -*----------------------------------------------------
 */

namespace crim {
    /**
     * @brief   Templated helper type alias for ease of use.
     *          See `/usr/include/c++/12/type_traits`.
     */
    template<typename T>
    using remove_cv_t = typename remove_cv<T>::type;

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
