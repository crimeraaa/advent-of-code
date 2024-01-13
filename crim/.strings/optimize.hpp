#include <stdexcept>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class cstring {
private:
    // Useful when we need to call const overloads for non-const instances.
    const cstring *const_this() {
        return const_cast<const cstring*>(this);
    }

    // Anonymous unions allow you to access to active member directly.
    // pvs-studio.com/en/blog/terms/6658/
    union {
        struct {
            char *p_memory;
            size_t n_count;
        } m_heapbuf; 
        char m_stackbuf[sizeof(m_heapbuf)]; 
    };
    size_t m_ncapacity;
public:
    // Start off with the assumption we'll only use the stack-allocated buffer.
    cstring(size_t n_length = 0) : m_stackbuf{0}, m_ncapacity{n_length} {} 

    // Add 1 to the strlen as it doesn't count the nul terminating character. 
    cstring(const char *p_string) : cstring(strlen(p_string)) {
        // Generalize our target so we don't write memcpy twice.
        char *p_buffer = m_stackbuf;
        size_t n_bytes = sizeof(char) * (m_ncapacity + 1);
        // Switch the active union member if needed.
        if (!is_short_string()) {
            // Set count to capacity as that's in bounds while bytes isn't.
            m_heapbuf = {(char*)malloc(n_bytes), m_ncapacity};
            if (m_heapbuf.p_memory == nullptr) {
                return; // TODO: Handle properly... somehow
            }
            p_buffer = m_heapbuf.p_memory;
        } 
        // Since bytes is cap + 1, this should already copy the nul terminator.
        memcpy(p_buffer, p_string, n_bytes);
    }
    
    ~cstring() {
        if (!is_short_string()) {
            free(m_heapbuf.p_memory);
        }
    }
    
    const char *data() const {
        return is_short_string() ? m_stackbuf : m_heapbuf.p_memory;
    }

    // Ugly af but works: https://stackoverflow.com/a/856839
    // Cast const to call const overload, then remove constness of return value.
    // This is OK because non-const values can always be promoted to const,
    // and since they weren't const to begin with casting it away is also OK.
    char *data() {
        return const_cast<char*>(const_this()->data());
    }
    
    const char &at(size_t n_index) const {
        const char *p_buffer = is_short_string() ? m_stackbuf : m_heapbuf.p_memory;
        size_t n_limit = is_short_string() ? m_ncapacity : m_heapbuf.n_count;
        if (n_index > n_limit) {
            throw std::out_of_range("cstring::at(): invalid index");
        }
        return p_buffer[n_index];
    }
    
    char &at(size_t n_index) {
        return const_cast<char&>(const_this()->at(n_index));
    }

    // STACKBUF:     I'm assuming capacity is based on strlen.
    // HEAPBUF:      Capacity will grow by multiples of 2 so use count. 
    size_t length() const {
        return is_short_string() ? m_ncapacity : m_heapbuf.n_count;
    }

    // STACKBUF:    Just the stack-allocated array's size, probably 16 chars.
    // HEAPBUF:     However many bytes have been allocated for it at this point. 
    size_t capacity() const {
        return is_short_string() ? sizeof(m_stackbuf) : m_ncapacity;
    }

    // Test if we haven't exceeded the m_stackbuf-allocated buffer yet. 
    bool is_short_string() const {
        return m_ncapacity < sizeof(m_stackbuf);
    }
};
