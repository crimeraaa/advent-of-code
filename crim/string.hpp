#pragma once

#include <stddef.h>
#include <string.h>
#include <stdexcept>

namespace crim {
    template<class CharT> class basic_string {
        static constexpr size_t STRING_BUFFER_START = 16;
        static constexpr size_t STRING_MAX_CAPACITY = 0xFFFFFF;
    private:
        size_t m_size; // bytes written to so far.
        size_t m_capacity; // bytes allocated in total.
        size_t *m_ncopies; // #copies made to avoid freeing the same memory
        CharT *m_buffer;
    public:
        // Primary constructor via `=`, or just an empty stirng
        basic_string(const CharT *message = "")
            : m_size{0}
            , m_capacity{STRING_BUFFER_START}
            , m_ncopies{new size_t}
            , m_buffer{new CharT[STRING_BUFFER_START]} 
        {
            *m_ncopies = 0;
            append(message);
        }

        // Copy-constructor
        basic_string(const basic_string &source) 
            : m_size{source.m_size}
            , m_capacity{source.m_capacity}
            , m_ncopies{new size_t}
            , m_buffer{new CharT[m_capacity]} 
        {
            *m_ncopies = 1;
            memcpy((void*)m_buffer, (void*)source.m_buffer, sizeof(CharT) * m_capacity);
        }

        ~basic_string() {
            delete m_ncopies;
            delete[] m_buffer;
        }

        // @bug Leaks memory! Probably on the call to resize.
        basic_string &append(const CharT *message) {
            // Don't reference at very start, may segfault
            if (m_size > 0 && m_buffer[m_size - 1] == '\0') {
                pop_back();
            }
            // Add 1 to len so we copy nul terminator
            for (size_t i = 0, len = strlen(message) + 1; i < len; i++) {
                push_back(message[i]);
            }
            // Calls the constructor ugh
            return *this;
        }

        void push_back(CharT elem) {
            if (m_size > STRING_MAX_CAPACITY) {
                throw std::length_error("Reached maximum string length!");
            }
            // capacity is 1-based, so if index = 1 and capacity = 1 realloc
            if (m_size + 1 > m_capacity) {
                m_capacity *= 2; // prolly faster this way also am lazy
                resize(m_capacity, m_size);
            }
            m_buffer[m_size++] = elem;
        }

        CharT pop_back() {
            return m_buffer[m_size--];
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

        /**
         * Returns your nul-terminated buffer.
         * @warning Don't modify else bad stuff may happen kthxbai
         * @note [See here](https://isocpp.org/wiki/faq/const-correctness#const-member-fns) 
         * as to why all the `const`.
         */
        const CharT *data() const {
            return m_buffer;
        }
    };

    using string = basic_string<char>;
    using wstring = basic_string<wchar_t>;
};
