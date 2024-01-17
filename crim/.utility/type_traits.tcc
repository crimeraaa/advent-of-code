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
 * -*- BEGIN REMOVE REFERENCE -*-
 * - https://en.cppreference.com/w/cpp/types/remove_reference
 */

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
 * -*- END REMOVE REFERENCE  -*-
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

/**
 * BEGIN CHAR TRAITS 
 */
#include "algorithm.tcc"
#include <cstring> /* std::memove */
#include <ios> /* std::streampos, std::streamoff */
#include <cwchar> /* std::mbstate_t */

namespace crim::base {
    template<typename CharT>
    struct char_types;
    
    template<typename CharT>
    struct char_traits;
};

/**
 * @brief   Simple mapping for character type to associated implementation types.
 *          If you need a different set of types, but don't need a different set
 *          of functions from `char_traits`, you can just specialize this.
 * 
 * @note    Mainly taken from the GNU Foundations's libstdc++ implementation.
 *          - https://en.cppreference.com/w/cpp/named_req/CharTraits
 *          - https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-api-4.5/a00783_source.html
  */
template<typename CharT>
struct crim::base::char_types {
    using int_type = unsigned long long;
    using pos_type = std::streampos;
    using off_type = std::streamoff;
    using state_type = std::mbstate_t;
};

/**
 * @brief   Base class which actually implements the C++ named requirement for
 *          CharTraits as seen in `std::char_traits`.
 *          Actually, it functions more like a templated namespace.
 *          - https://en.cppreference.com/w/cpp/named_req/CharTraits
 *          - https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-api-4.5/a00783_source.html
 *
 * @note    This class is designed to be specialized as the internal types
 *          are likely wrong for most character types that aren't raw `char`.
 */
template<typename CharT>
struct crim::base::char_traits {
    // Stupid, but probably used by other standard library functions and such.
    using char_type  = CharT;

    // Helper typedef because the rest of the C++ STL does this for some reason.
    using reference = char_type&;
    using pointer = char_type*;
    using size_type = typename std::size_t;
    
    // Need this since `const reference` refers to `char_type &const`.
    using const_reference = const char_type&;

    // Need this since `const pointer` refers to `char_type *const`.
    using const_pointer = const char_type*;
    
    // Can hold all values of `char_type` alongside their `EOF` counterpart.
    using int_type = typename char_types<CharT>::int_type;

    // Invokes implementation-defined behvaiour when not `std::streamoff`
    // and `char_types` is used as the traits template parameter for I/O.
    using pos_type = typename char_types<CharT>::pos_type;

    // Signed integral representing the maximum possible file size supported
    // by the host OS. Value of `-1` is often used to signal errors.
    using off_type = typename char_types<CharT>::off_type;

    // A struct which is a non-trivial array type to represent conversion states
    // in multibyte character encoding rules. Most is implementation defined.
    // Must be destructible, copy assignable and copy/move constructible.
    using state_type = typename char_types<CharT>::state_type;
    
    static void assign(reference ch_dst, const_reference ch_src)
    {
        ch_dst = ch_src;
    }
    
    static bool eq(const_reference ch_left, const_reference ch_right)
    {
        return ch_left == ch_right;
    }
    
    static bool lt(const_reference ch_left, const_reference ch_right)
    {
        return ch_left < ch_right;
    }
    
    static int compare(const_pointer c_str1, const_pointer c_str2, size_type len)
    {
        for (size_type i = 0; i < len; i++) {
            if (lt(c_str1[i], c_str2[i])) {
                // str1 is "lower" in ASCII value than c_str2 at this position
                return -1; 
            } else if (lt(c_str2[i], c_str1[i])) {
                // Vice versa
                return 1; 
            }
        }
        return 0;
    }
    
    // Basic string length for any sequence of nul-terminated kharacters.
    static size_type length(const_pointer c_str)
    {
        // Default constructor should 0 out memory, so should be '\0'.
        static constexpr char_type nul = char_type();
        size_type len = 0;
        while (!eq(c_str[len], nul)) {
            ++len;
        }
        return len;
    }
    
    // Get a pointer to the first occurence of `ch` in `c_str`, or `nullptr`.
    static const_pointer find(const_pointer c_str, size_type len, const_reference ch)
    {
        for (size_type idx = 0; idx < len; idx++) {
            if (eq(c_str[idx], ch)) {
                return c_str + idx; // Absolute address via pointer arithmetic
            }
        }
        return nullptr;
    }
    
    static pointer move(pointer c_str1, const_pointer c_str2, size_type len)
    {
        void *ptr = std::memmove(c_str1, c_str2, sizeof(char_type) * len);
        return static_cast<char_type*>(ptr);
    }
    
    static pointer copy(pointer c_str1, const_pointer c_str2, size_type len)
    {
        crim::copy(c_str2, c_str2 + len, c_str1);
        return c_str1;
    }
    
    static pointer assign(pointer c_str, size_type len, char_type ch)
    {
        crim::fill_n(c_str, len, ch);
        return c_str;
    }
};
/**
 * END CHAR TRAITS 
 */
