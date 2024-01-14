#pragma once

/**
 * Due to how macro expansion works, e.g. for the `__LINE__` macro,
 * we use this to convert the expanded value of that macro to a string.
 *
 * Without this the stringification of `__LINE__` returns `"__LINE__"`.
 * But the desired behaviour is for it to exapnd to, say, `"62"`.
 * - https://stackoverflow.com/a/2670913
 */
#define crim_stringify(expanded_macro) #expanded_macro

/**
 * Create a concatenated string literal of the file name and a line number.
 * e.g. `"crim/base_dyarray.tcc:62@"`
 * 
 * This is stupid but it seems standardized enough.
 *  - https://learn.microsoft.com/en-us/cpp/preprocessor/stringizing-operator-hash?view=msvc-170
 *  - https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
 */
#define crim_loginfo(line) __FILE__ ":" crim_stringify(line) "\n\t"

/**
 * Creates a pretty-printable string literal. For example:
 * ```cpp
 * crim_make_logmsg("main", "allocation failure") 
 * // Expands to:
 * "./crim/test.cpp" ":" "13" "\\n\\t" "main" "(): " "allocation failure"
 * // Which in turn expands to: 
 * "./crim/test.cpp:13" 
 * "-------- main(): allocation failure"
 * ```
 * 
 * Note however, that we can't use the `__func__` or `__FUNCTION__` macros here.
 * This is because they are not really macros or variables.
 * As a result, they cannot be concatenated properly.
 *  - https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Function-Names.html
 *  - https://stackoverflow.com/a/48125024
 */
#define crim_make_logmsg(fn, info) crim_loginfo(__LINE__) fn "(): " info

#if defined(CRIM_LOGERROR_USE_STDERR) || !defined(CRIM_LOGERROR_USE_CERR)
#include <cstdio>
static inline void crim_logerror_cstdio(const char *p_errmsg) {
    std::fprintf(stderr, "%s\n", p_errmsg);
}
#define crim_logger_fn(message) crim_logerror_cstdio(message)
#else 
#include <iostream>
static inline void crim_logerror_stdcerr(const char *p_errmsg) {
    std::cerr << p_errmsg << "\n";
}
#define crim_logger_fn(message) crim_logerror_stdcerr(message)
#endif

/**
 * @brief   Print a pretty-printed error message to `stderr` or `std::cerr`.
 *          The error message includes the file name and line number.
 *
 * @param   fn      Function name that `__func__` would contain.
 * @param   info    Some extra messages/info you'd like to send out.
 * 
 * @note    By default, we use `std::fputs` and `stderr`. 
 *          To use `std::cerr`, define the `CRIM_LOGERRROR_USE_CERR` macro
 *          before including this header.
 */
#define crim_logerror(fn, info) crim_logger_fn(crim_make_logmsg(fn, info))
