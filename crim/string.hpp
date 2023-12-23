#pragma once

#include <stddef.h>
#include <string.h>
#include <stdexcept>

namespace crim {
    template<class CharT> class basic_string;
};

template<class CharT> class crim::basic_string {
    static constexpr size_t STRING_BUFFER_START = 16;
    static constexpr size_t STRING_MAX_CAPACITY = 0xFFFFFF;
private:
    size_t m_index; // Number of elements written `m_buffer` so far.
    size_t m_capacity; // Number of elements `m_buffer` can hold in total.
    CharT *m_buffer; // Heap-allocated 1D array of the specified character type.
public:
    // ------------------ CONSTRUCTORS/DESTRUCTORS -----------------------------

    // Primary constructor via `=`, or just an empty stirng
    basic_string(const CharT *message = "")
        : m_index{0}
        , m_capacity{STRING_BUFFER_START}
        , m_buffer{new CharT[STRING_BUFFER_START]} 
    {
        append(message);
    }

    /**
     * @brief Copies `source` into a new `basic_string` instance.
     * The instance receives a deep copy of the source's buffer, not shallow.
     */
    basic_string(const basic_string &source) 
        : m_index{source.m_index}
        , m_capacity{source.m_capacity}
        , m_buffer{new CharT[m_capacity]}
    {
        // Both memory blocks don't overlap, is prolly faster than strcpy
        // is more generic anyway so I don't need to use wstrcpy or some crap
        memcpy(m_buffer, source.m_buffer, sizeof(CharT) * m_capacity);
    }

    ~basic_string() {
        delete[] m_buffer;
    }

    // -------------------- CONTAINER INFORMATION ------------------------------

    // Number of elements written to the internal buffer so far.
    size_t length() {
        return m_index;
    }

    // Maximum number of elements the internal buffer can hold currently.
    size_t capacity() {
        return m_capacity;
    }

    // Maximum allowable number of elements that can be written to the buffer.
    static constexpr size_t maxsize() {
        return STRING_MAX_CAPACITY;
    }

    /**
     * Returns your nul-terminated buffer.
     * @warning Don't modify else bad stuff may happen kthxbai
     * @note [See here](https://isocpp.org/wiki/faq/const-correctness#const-member-fns) 
     * as to why all the `const`.
     */
    const CharT *data() const {
        return m_buffer;
    }

    // ----------------------- BUFFER WRITERS ----------------------------------

    /**
     * @brief Appends `message` to the caller's internal buffer.
     * @note This may reallocate memory for the internal buffer via `resize`.
     */
    basic_string &append(const CharT *message) {
        // Don't reference at very start, may segfault
        if (m_index > 0 && m_buffer[m_index - 1] == '\0') {
            pop_back();
        }
        // Add 1 to len so we copy nul terminator
        for (size_t i = 0, len = strlen(message) + 1; i < len; i++) {
            push_back(message[i]);
        }
        // This calls the constructor but you know what? idc anymore :)
        return *this;
    }

    /**
     * @brief Appends the `elem` to the end of the buffer.
     * @exception `std::length_error` when internal limit is reached.
     * @note May resize the internal buffer.
     */
    void push_back(CharT elem) {
        if (m_index > STRING_MAX_CAPACITY) {
            throw std::length_error("Reached maximum string length!");
        }
        // capacity is 1-based, so if index = 1 and capacity = 1 realloc
        if (m_index + 1 > m_capacity) {
            m_capacity *= 2; // prolly faster this way also am lazy
            resize(m_capacity, m_index);
        }
        m_buffer[m_index++] = elem;
    }

    /**
     * @brief Removes and returns to you the last element in the buffer.
     * @note Sets that last position in the buffer to the nul character.
     */
    CharT pop_back() {
        CharT last = m_buffer[m_index];
        m_buffer[m_index--] = (CharT)'\0';
        return last;
    }

    /**
     * Basically `realloc` in C.
     * @param newcap  How much new memory to allocate, can be more or less.
     * @param newsize Up to how many elements to copy.
     * @warning You can overwrite memory if you're not careful!
     */
    void resize(size_t newcap, size_t newsize) {
        CharT *tmp = new CharT[newcap];
        for (size_t i = 0; i < newsize; i++) {
            tmp[i] = m_buffer[i];
        }
        delete[] m_buffer;
        m_buffer = tmp;
    }
};

// Template definition must be in scope for template instances to work
namespace crim {
    using string = basic_string<char>;
    using wstring = basic_string<wchar_t>;
};
