#pragma once

#include "base_dyarray.tcc"

namespace crim {
    template<class CharT> class dystring;
};

/**
 * @brief   A growable 1D array of a character type with many helper methods
 *          for manipulating the string.
 * 
 * @tparam  CharT Desired character type, e.g. `char`, `wchar_t`, etc.
 */
template<typename CharT> 
class crim::dystring : public crim::base_dyarray<crim::dystring<CharT>, CharT> {
private:
    // Use this for quick access to base's methods and casts to the base class.
    using base = base_dyarray<dystring<CharT>, CharT>;
public:
    // -------------------- CONSTRUCTORS/DESTRUCTORS ---------------------------

    /**
     * @brief   Appends `msg` to the already-allocated internal buffer.
     * 
     *          This is because the constructor for `base::base()` would
     *          have already run, so all inherited members are initialized.
     * 
     * @param   msg     nul-terminated C-string (preferably a literal).
     */
    dystring(const CharT *msg = "") : base() {
        append(msg);
    }

    /**
     * @brief   Copy-constructor which delegates to the base class's version.
     *          That one takes care of doing the deep copy for us.
     * 
     *          Expect that this will be used a lot internally, especially for
     *          return values.
     * 
     * @note    We specifically call the base class copy-constructor here.
     *          Since without it, the compiler will cry and I'll be very sad.
     */
    dystring(const dystring &src) : base(src) {
        append('\0');
    }

    /**
     * @brief   Move-constructor which delegates to the base class's version. 
     * 
     *          It does a shallow copy, sets `src`'s pointers to `nullptr`.
     *          Upon temporary `src`'s destruction, the memory pointed to by
     *          `src.m_buffer` is not freed, allowing us to keep it around!
     */
    dystring(dystring &&src) : base(std::forward<base>(src)) {
        append('\0');
    }

    // ----------------------- ASSIGNMENT OPERATORS ----------------------------

    /**
     * @brief   Instance reassignment. If you reassign an instance to a new 
     *          C-style string literal, we'll empty the buffer first before we
     *          copy over `msg` into the buffer.
     */
    dystring &operator=(const CharT *msg) {
        dystring tmp = msg; // basic constructor
        std::swap(*this, tmp);
        return *this;
    }

    /**
     * @brief   Copy-assignment, effectively "delegates" to the base class's.
     * 
     *          Deep copies of `src`'s data including the buffer. This means that
     *          `this->m_buffer` & `src.m_buffer` are distinct blocks of memory.
     * 
     * @details `base::m_length` doesn't include the nul char in its count, we
     *          have to explicitly append it ourselves else we might end up with
     *          a likely buffer overrun!
     * 
     * @note    Since this is a copy, both this and the `base::copy` method need
     *          `src` to be const to ensure it is immutable for the entire call.
     * 
     */
    dystring &operator=(const dystring &src) {
        dystring tmp = src; // copy-constructor as to not mess up `src`
        std::swap(*this, tmp.append('\0'));
        return *this;
    }

    /** 
     * @brief   Move-assignment, effectively "delegates" to the base class's.
     * 
     *          Does a shallow copy of `src`'s data. So `this->m_buffer` points
     *          to the same data that `src.m_buffer` does.
     * 
     * @details Strangely, `base::move` can't take an rvalue reference if we're
     *          calling it from here. It can take a good old lvalue reference.
     * 
     *          This is probably since `src`, as a parameter, is already named.
     *          So when passed to other functions it's treated as an lvalue?
     * 
     * @details `base::m_length` doesn't include the nul char in its count, we
     *          have to explicitly append it ourselves else we might end up with
     *          a likely buffer overrun!
     * 
     * @note    `src` is modified later so that its buffer points to null. We do
     *          this to avoid freeing allocated memory of temporary instances.
     */
    dystring &operator=(dystring &&src) {
        base::move(std::forward<base>(src));
        return append('\0');
    }

    dystring &operator+=(const CharT *msg) {
        return append(msg);
    }

    dystring &operator+=(const dystring &src) {
        return append(src.c_str());
    }

    // ----------------------- DATA ACCESS OPERATORS --------------------------- 
    
    /**
     * @brief   Read-only nul-terminated C-string for you to print out.
     * 
     * @warning Don't modify the contents or else you'll have a bad time kthxbai
     * 
     * @note    `base::data()` does the same thing, and we call it here!
     */
    const CharT *c_str() const {
        return base::data();
    }

    // ---------------------- STRING WRITING FUNCTIONS -------------------------

    /**
     * @brief   Removes the most recently written element from the buffer.
     *          Decrements index counter to indicate this index is writeable.
     * 
     * @note    Shadows `base::pop_back()` so that we can set the popped index
     *          to a nul char.
     */
    CharT pop_back() {
        CharT ch_top = base::pop_back();
        append('\0'); // this index is now writeable
        return ch_top;
    }

    /**
     * @brief   Appends nul terminated `msg` to our internal buffer.
     * 
     *          This will automatiically erase the previous nul terminator then 
     *          add one at the very end without counting it in the index.
     * 
     * @note    Might reallocate memory for the internal buffer via `resize`,
     *          which is called in the parent method `base::push_back()`.
     */
    dystring &append(const CharT *msg) {
        // Don't add 1 to avoid writing nul char and incrementing index both.
        for (size_t i = 0, len = strlen(msg); i < len; i++) {
            base::push_back(msg[i]);
        }
        return append('\0');
    }

    /**
     * @brief   Appends a single character to the buffer, and then a nul char.
     * 
     * @details Inserting the nul char doesn't increment the index. If `c` is a
     *          nul char to begin with, we'll do the same. This is to ensure the
     *          string's contents are always nul terminated.
     * 
     * @note    Might reallocate memory for the internal buffer via `resize`,
     *          which is called in the parent method `base::push_back()`.
     */
    dystring &append(CharT c) {
        if (c != '\0') {
            base::push_back(c);
        }
        // Decrement the index so we effectively ignore nul chars in our count.
        base::push_back('\0');
        base::m_length--;
        base::m_iterator.m_end--; // push_back incremented this so decrement it
        return *this;
    }
};

namespace crim {
    // C-style dynamic string (basic character array container).
    using string = dystring<char>;

    // C-style dynamic wide-string (wide-character array container).
    // Prepend string literals of this type with `L`, like `L"Hi mom!"`.
    using wstring = dystring<wchar_t>;

// char8_t was only introduced in C++20.
// https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
#if __cplusplus >= 202002L
    // A C-style dynamic UTF-8 string (UTF-8 encoded character array container).
    using u8string = dystring<char8_t>;
#endif // __cplusplus >= 202002L

// char16_t and char32_t were introduced in C++11.
#if __cplusplus >= 201103L
    // C-style dynamic UTF-16 string (UTF-16 encoded character array container).
    // Prepend string literals of this type with `u`, like `u"Hi mom!"`
    using u16string = dystring<char16_t>;
    
    // C-style dynamic UTF-32 string (UTF-32 encoded character array container).
    // Prepend string literals of this type `u`, like `u"Hi mom!"`
    using u32string = dystring<char32_t>;
#endif // __cplusplus >= 201103L
};
