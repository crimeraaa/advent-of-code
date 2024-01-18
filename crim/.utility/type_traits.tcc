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
 * BEGIN BASE CHAR TRAITS 
 */
#include "algorithm.tcc"
#include <cstring> /* std::memove */
#include <ios> /* std::streampos, std::streamoff */
#include <cwchar> /* WEOF, std::mbstate_t */
#include <cstdio> /* EOF */

#define CRIM_CHAR_TRAITS_EOF EOF

namespace crim::impl {
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
struct crim::impl::char_types {
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
struct crim::impl::char_traits {
    // Stupid, but probably used by other standard library functions and such.
    using char_type = CharT;

    // No way in hell I'm typing all that out.
    using base = crim::impl::char_traits<char_type>;

    // Helper typedef because the rest of the C++ STL does this for some reason.
    using reference = char_type&;
    using pointer = char_type*;
    using size_type = typename std::size_t;
    
    // Need these since `const reference` refers to `char_type &const`.
    using const_reference = const char_type&;
    
    // `const pointer` refers to `char_type *const`.
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
    
    /**
     * @brief   Generic function which copy-assigns `src` to `dst`. 
     */
    static void assign(reference dst, const_reference src)
    {
        dst = src;
    }
    
    /**
     * @brief       Case-sensitive equality comparison for one character.
     *              By default we compare them by their internal representation,
     *              such as ASCII values for `char`.
     */
    static bool eq(const_reference ch1, const_reference ch2)
    {
        return ch1 == ch2;
    }
    
    /**
     * @brief       Case-sensitive less-than comparison for one character.
     *              By default we compare them by their internal representation,
     *              such as ASCII values for `char`.
     */
    static bool lt(const_reference ch1, const_reference ch2)
    {
        return ch1 < ch2;
    }
    
    /**
     * @brief       Case-sensitive equality comparison between 2 nul terminated
     *              strings of type `CharT`.
     *
     * @return      `-1` indicates `s1` is "less than" `s2`.
     *              `1` indicates `s1` is "more than" `s2`.
     *              `0` indicates that both strings are exactly the same.
     */
    static int compare(const_pointer s1, const_pointer s2, size_type len)
    {
        for (size_type i = 0; i < len; i++) {
            if (lt(s1[i], s2[i])) {
                // str1 is "lower" in ASCII value than s2 at this position
                return -1; 
            } else if (lt(s2[i], s1[i])) {
                // Vice versa
                return 1; 
            }
        }
        return 0;
    }
    
    /**
     * @brief   Basic string length for any sequence of nul-terminated characters.
     *          
     * @note    If a dedicated library functions exists for a given type,
     *          e.g. `std::strlen` or `std::wcslen`, specialize this function.
     */
    static size_type length(const_pointer s)
    {
        // Default constructor should 0 out memory, so should be '\0'.
        static constexpr char_type nul{};
        size_type len = 0;
        while (!eq(s[len], nul)) {
            ++len;
        }
        return len;
    }
    
    /**
     * @brief   Find the first occurence of `ch` in `s`.
     * 
     * @return  Pointer to occurence or `nullptr`.
     */
    static const_pointer find(const_pointer s, size_type len, const_reference ch)
    {
        for (size_type idx = 0; idx < len; idx++) {
            if (eq(s[idx], ch)) {
                return s + idx; // Absolute address via pointer arithmetic
            }
        }
        return nullptr;
    }
    
    static pointer move(pointer dst, const_pointer src, size_type len)
    {
        void *ptr = std::memmove(dst, src, sizeof(char_type) * len);
        return static_cast<pointer>(ptr);
    }
    
    /**
     * @brief   Copies `len` bytes of `src` into `dst` using `std::memcpy`.
     */
    static pointer copy(pointer dst, const_pointer src, size_type len)
    {
        void *ptr = std::memcpy(dst, src, sizeof(char_type) * len);
        return static_cast<pointer>(ptr);
    }
    
    /**
     * @brief   Fills range `s` to `s + len` with `ch` using `std::memset`.
     */
    static pointer assign(pointer s, size_type len, char_type ch)
    {
        void *ptr = std::memset(s, ch, sizeof(char_type) * len);
        return static_cast<pointer>(ptr);
    }
    
    static char_type to_char_type(const int_type &ch)
    {
        return static_cast<char_type>(ch);
    }
    
    static bool eq_int_type(const int_type &c1, const int_type &c2)
    {
        return c1 == c2;
    }

    /**
     * @brief   The end-of-file return value used by library functions for the 
     *          given type. 
     * 
     * @note    Note that `wchar_t` MUST use `WEOF`.
     *          For other types, you may need to specialize this.
     */
    static int_type eof()
    {
        return static_cast<int_type>(CRIM_CHAR_TRAITS_EOF);
    }
    
    /**
     * @brief   Check if `eof()` is not true.
     * 
     * @return  0 if `eof()` is true, else the value of `ch`.
     */
    static int_type not_eof(const int_type &ch)
    {
        return (ch == eof()) ? 0 : ch;
    }
};
/**
 * END BASE CHAR TRAITS 
 */

/**
 * BEGIN CHAR TRAITS SPECIALIZATIONS 
 */

namespace crim {
    template<typename CharT>
    struct char_traits;
};

/**
 * @brief   Base class for explicit traits specializations.
 *          Probably wrong for actual character types, this is just a thin
 *          wrapper around `crim::impl::char_traits`.
 *          - https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-api-4.5/a00783_source.html
 *
 * @note    You're better off creating explicit specializations of this alongside
 *          `crim::impl::char_types`.
 */
template<typename CharT>
struct crim::char_traits : public crim::impl::char_traits<CharT> {};

/**
 * BEGIN SPECIALIZATION: char 
 */

/** 
 * @brief   `crim::impl::char_traits` uses typenames from `crim::impl::char_types`.
 *          So specializing this ensures our inherited types are correct. 
 */
template<> struct crim::impl::char_types<char> {
    using char_type = char;
    using int_type = int;
    using pos_type = std::streampos;
    using off_type = std::streamoff;
    using state_type = std::mbstate_t;
};

/**
 * @brief   Template specialization of `crim::char_traits<T>` for `char`. 
 *          By inheriting the base class publicly, we have access to many of the
 *          predefined functions that we did not specialize.
 *          So common functions like `copy` and `move` are inherited.
 */
template<> 
struct crim::char_traits<char> : public crim::impl::char_traits<char> {
    /**
     * Bring `assign(char &dst, const char &src)` into scope for usage.
     *
     * This is because when we specialize `assign(char *p, size_t len, char ch)`
     * by default we don't look for any matches/overloads from the impl class.
     *  - https://stackoverflow.com/a/14212227
     */
    // using base::assign;

    /**
     * @brief   Compare `s1` and `s2` case-sensitively.
     * 
     * @return  == 0:   both strings are the same.
     *          >  0:   s1's first differing character is higher than s2's.
     *          <  0:   s1's first differing character is lower than s2's.
     */
    static int compare(const_pointer s1, const_pointer s2, size_type len)
    {
        return std::memcmp(s1, s2, len);
    }
    
    /**
     * @brief   String length, duh. Specialization to call `std::strlen`.
     * 
     * @note    `std::strlen` will ONLY work for char types, hence the need for
     *          the specialization.
     */
    static size_type length(const_pointer s)
    {
        return std::strlen(s);
    }
    
    /**
     * @brief   Find first occurence of `ch` in `s`. Calls `std::memchr`.
     */
    static const_pointer find(const_pointer s, size_type len, const_reference ch)
    {
        const void *p = std::memchr(s, ch, len);
        return static_cast<const_pointer>(p);
    }
    
};
/**
 * END SPECIALIZATION: char 
 */

/**
 * BEGIN SPECIALIZATION: wchar_t 
 */

template<>
struct crim::impl::char_types<wchar_t> {
    using char_type = wchar_t;
    using int_type = wint_t;
    using pos_type = std::streampos;
    using off_type = std::streamoff;
    using state_type = std::mbstate_t;
};

template<>
struct crim::char_traits<wchar_t> : public crim::impl::char_traits<wchar_t> {
    /**
     * @brief   Compare `s1` and `s2` case-sensitively up to `len` wide characters.
     *          We rely on `std::wmemcmp`, maybe it's more efficient.
     */
    static int compare(const_pointer s1, const_pointer s2, size_type len)
    {
        return std::wmemcmp(s1, s2, len);        
    }
    
    /**
     * @brief   String length, duh. Relies on `std::wcslen`.
     */
    static size_type length(const_pointer s)
    {
        return std::wcslen(s);
    }
    
    static const_pointer find(const_pointer s1, size_type len, const_reference ch)
    {
        return std::wmemchr(s1, ch, len);
    }
    
    static pointer move(pointer dst, const_pointer src, size_type len)
    {
        return std::wmemmove(dst, src, len);
    }
    
    static pointer copy(pointer dst, const_pointer src, size_type len)
    {
        return std::wmemcpy(dst, src, len);
    }
    
    static pointer assign(pointer s, size_type len, char_type ch)
    {
        return std::wmemset(s, ch, len);
    }
    
    /**
     * @brief   The end-of-file return value, `WEOF`, given by library functions
     *          like `fgetwc()`. 
     * 
     * @note    Specialized from the base `eof()` which would otherwise just 
     *          return the normal `EOF`.
     */
    static int_type eof()
    {
        return static_cast<int_type>(WEOF);
    }
};

/**
 * END SPECIALIZATION: wchar_t
 */

#undef CRIM_CHAR_TRAITS_EOF

/**
 * END CHAR TRAITS SPECIALIZATIONS
 */
