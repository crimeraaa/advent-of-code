#pragma once

#include <stddef.h>
#include <string.h>
#include <stdexcept>

#include "dyarray.hpp"

namespace crim {
    /**
     * A growable string buffer.
     * @tparam CharT A character type, e.g. `char` or `wchar_t`.
     */
    template<typename CharT> class basic_string : public dyarray<CharT> {
        // dyarray<CharT> *base = static_cast<dyarray<CharT>*>(this); // lol
    public:
        // If no immediate assignment, start with an empty string, but size 0.
        basic_string() {
            this->m_buffer[0] = '\0';
        }

        // Initialize your string with some message.
        basic_string(const CharT *message) {
            append(message);
        }

        /**
         * Returns a pointer to the start of your stored character buffer. 
         * @warning Don't modify pls else bad bad stuff will happen okthxbai :)
         * @note Remember that it is (or should be, anyway) nul terminated!
         */
        const CharT *c_str() {
            return this->m_buffer;
        }

        // Assigns some message into your buffer directly.
        basic_string &operator=(const CharT *source) {
            // Buffer will (hopefully) never be unitialized
            return append(source);
        }

        // Appends the given text to your string buffer and removes leading
        // nul termination.
        basic_string &operator+=(const CharT *source) {
            if (this->at(this->size() - 1) == '\0') {
                this->pop_back();
            }
            return append(source);
        }

        bool compare(const basic_string &lhs, const basic_string &rhs) {
            if (lhs.m_size != rhs.m_size) {
                return false;
            }
            for (size_t i = 0; i < lhs.m_size; i++) {
                if (lhs.m_buffer[i] != rhs.m_buffer[i]) {
                    return false;
                }
            }
            return true;
        }

        // Probably won't work for temporary instances but hey
        bool operator==(const basic_string &rhs) {
            return compare(*this, rhs);
        }

        // Appends the given text to your string buffer.
        basic_string &append(const CharT *source) {
            // Remember that strlen doesn't include the nul char!
            for (size_t i = 0, len = strlen(source) + 1; i < len; i++) {
                this->push_back(source[i]);
            }
            // append nul for safety
            if (this->at(this->size() - 1) != '\0') {
                this->push_back('\0'); 
            }
            return *this;
        }
    };

    // ----------------- STRING TYPE TEMPLATE INSTANCES ------------------------
    // For samples, see wherever your C++ <string> implementation is found.

    // A C-style array of `char` with nul-termination and some helper methods.
    using string = basic_string<char>;

    // Similar to `crim::string` but specialized for `wchar_t` or Unicode chars.
    using wstring = basic_string<wchar_t>;
};
