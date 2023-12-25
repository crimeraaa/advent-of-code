#pragma once

#include "base_dyarray.hpp"

namespace crim {
    template<class CharT> class dystring;
};

/**
 * @brief   A growable 1D array of a character type with many helper methods
 *          for manipulating the string.
 * 
 * @tparam  CharT Desired character type, e.g. `char`, `wchar_t`, etc.
 */
template<class CharT> 
class crim::dystring : public crim::base_dyarray<crim::dystring<CharT>, CharT> {
private:
    // No way in hell I am typing all that
    using base_class = base_dyarray<dystring<CharT>, CharT>;

    // base_class *m_baseptr = static_cast<base_class*>(this);

public:
    // -------------------- CONSTRUCTORS/DESTRUCTORS ---------------------------

    /**
     * @brief   Appends `message` to the already-allocated internal buffer.
     * 
     *          This is because the constructor for `crim::base_dyarray` would
     *          have already run, so all inherited members are initialized.
     * 
     * @param   message     nul-terminated C-string (preferably a literal).
     */
    dystring(const CharT *message = "") {
        append(message);
    }

    /**
     * @brief   Copy-constructor which does a deep copy of the internal buffer.
     * 
     *          Expect that this will be used a lot internally, especially for
     *          return values.
     * 
     * @note    We specifically need to call the base class constructor here.
     *          Without it, the compiler will cry and I'll be very sad.
     */
    dystring(const dystring &source) : base_class() {
        append(source.c_str());
    }

    // ----------------------- ASSIGNMENT OPERATORS ----------------------------

    // Clear the previous buffer and reassign its contents to contain `message`.
    dystring &operator=(const CharT *message) {
        return this->clear().append(message);
    }

    // Outside of declaration, deep-copy a dystring's contents.
    dystring &operator=(const dystring &source) {
        return this->clear().append(source.c_str());
    }

    // Concatenate the nul-terminated C-style string `message` to our buffer.
    dystring &operator+=(const CharT *message) {
        return append(message);
    }

    // Concatenate `source`'s nul-terminated C-style string into our buffer.
    dystring &operator+=(const dystring &source) {
        return append(source.c_str());
    }

    // ----------------------- DATA ACCESS OPERATORS --------------------------- 
    
    /**
     * @brief   Read-only nul-terminated C-string for you to print out.
     *          Please don't modify or else you gonna have a bad time kthxbai
     * 
     * @note    `crim::base_dyarray::data()` functionally does the same thing.
     */
    const CharT *c_str() const {
        return this->m_buffer;
    }

    // ---------------------- STRING WRITING FUNCTIONS -------------------------

    /**
     * @brief   Removes the most recently written element from the buffer.
     *          Decrements index counter to indicate this index is writeable.
     * 
     * @note    Override base_dyarray one so we can assign nul at the index.
     *          Note that this does not affect `m_capacity` or any allocations.
     */
    CharT pop_back() {
        CharT ch_top = this->m_buffer[this->m_size];
        this->m_buffer[this->m_size--] = '\0';
        return ch_top;
    }

    /**
     * @brief   Appends nul terminated `message` to our internal buffer.
     * 
     *          This will automatiically erase the previous nul terminator then 
     *          add one at the very end without counting it in the index.
     * 
     * @note    Might reallocate memory for the internal buffer via `resize`,
     *          which is called in `crim::base_dyarray::push_back()`.
     */
    dystring &append(const CharT *message) {
        // Don't add 1 so we don't write nul char AND increment index
        for (size_t i = 0, len = strlen(message); i < len; i++) {
            this->push_back(message[i]);
        }
        // We want to exclude the nul char from the count.
        this->m_buffer[this->m_size] = (CharT)'\0';
        return *this;
    }

    dystring &append(CharT c) {
        this->push_back(c);
        // We want to exlucde the nul char from the count.
        this->m_buffer[this->m_size] = (CharT)'\0';
        return *this;
    }
};

namespace crim {
    // C-style dynamic string (basic character array).
    using string = dystring<char>;

    // C-style dynamic wide-string (wide-character array).
    // Prepend literals with `L`, like `L"Hi mom!"`.
    using wstring = dystring<wchar_t>;

    // char8_t was only introduced in C++20.
    // https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
    #if __cplusplus >= 202002L

    // A C-style dynamic UTF-8 string (UTF-8 encoding character array).
    using u8string = dystring<char8_t>;

    #endif // __cplusplus >= 202002L

    // char16_t and char32_t were introduced in C++11.
    #if __cplusplus >= 201103L

    // C-style dynamic UTF-16 string (UTF-16 encoding character array).
    // Prepend literals with `u`, like `u"Hi mom!"`
    using u16string = dystring<char16_t>;
    
    // C-style dynamic UTF-32 string (UTF-32 encoding character array).
    // Prepend literals with `u`, like `u"Hi mom!"`
    using u32string = dystring<char32_t>;

    #endif // __cplusplus >= 201103L

};
