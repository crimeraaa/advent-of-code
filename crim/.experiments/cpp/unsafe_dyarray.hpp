/* -*- C STANDARD LIBRARY -*- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

namespace crim {
    namespace unsafe {
        void logerror(const char *file, int line, const char *name, const char *info) {
            fprintf(stderr, "%s:%i @ %s: %s\n", file, line, name, info);
        }
        class dyarray;
    };
};

// Quick 'n dirty macro to avoid constantly typing `__FILE__` and `__LINE__`.
#define ga_logerror(name, info) \
    logerror(__FILE__, __LINE__, "dyarray::" name, info)

class crim::unsafe::dyarray {
private:
    // This is a terrible idea but I wanna see how bad it gets ^_^
    using byte = unsigned char;
    using cb_create = void *(*)(void *p_dest, size_t n_sizeof);
    using cb_copy = void *(*)(void *p_dest, const void *p_src, size_t n_sizeof);

    byte *m_pbuffer; // 1D heap-allocated array of whatever.
    size_t m_ncount; // 1-based current number of elements stored in buffer.
    size_t m_ncapacity; // 1-based amount of elements buffer can store.
    size_t m_nsizeof; // Size of the desired type. Very important!!!
    cb_create m_create; // Callback function, if nullptr we `memset` w/ 0.
    cb_copy m_copy; // Callback function, if nullptr we just do a memcpy.
public:
    /* -*- "CONSTRUCTOR", "DESTRUCTOR" -*- */

    void init(size_t n_sizeof, cb_create create_fn = nullptr, cb_copy copy_fn = nullptr) {
        m_pbuffer = nullptr;
        m_ncount = 0;
        m_ncapacity = 0;
        m_nsizeof = n_sizeof;
        m_create = (create_fn != nullptr) ? create_fn : [](void *p_dest, size_t n_sizeof)->void* {
            return memset(p_dest, 0, n_sizeof);
        };
        m_copy = (copy_fn != nullptr) ? copy_fn : memcpy;
    }

    void deinit() {
        free(m_pbuffer);
    }
    
    /* -*- DATA ACCESS -*- */
    
    size_t length() const {
        return m_ncount;
    }
    
    size_t capacity() const {
        return m_ncapacity;
    }
    
    size_t itemsize() const {
        return m_nsizeof;
    }
    
    byte *get_address_of(size_t n_index) const {
        return m_pbuffer + (n_index * m_nsizeof);
    }

    // TODO: Allow for user-defined construct/init function for non-trivials
    void copy_to(void *p_dest, const void *p_src) {
        m_copy(p_dest, p_src, m_nsizeof);
    }

    // Pass the address of a stack-allocated variable of your desired type.
    // @warning Only checks for out of bounds, for anything else good luck!
    void at(size_t n_index, void *p_dest) {
        // m_ncount itself is 1 past the most recently written element.
        if (n_index >= m_ncount) {
            crim_allocator_logerror("at", "invalid index");
            return;
        }
        copy_to(p_dest, get_address_of(n_index));
    }
    
    bool resize(size_t n_newcap) {
        if (n_newcap == m_ncapacity) {
            return true;
        }
        // If shortening the buffer, only copy newcap number of elements instead
        if (n_newcap < m_ncount) {
            m_ncount = n_newcap;
        }
        // Update so we can allocate the correct amount of memory
        m_ncapacity = n_newcap;
        byte *p_dummy = (byte*)malloc(m_nsizeof * m_ncapacity);
        if (p_dummy == nullptr) {
            return false;
        }
        // TODO: Use with non-trivial init functions (can't just memcpy)
        for (size_t i = 0; i < m_ncount; i++) {
            size_t offset = i * m_nsizeof;
            copy_to(p_dummy + offset, m_pbuffer + offset);
        }
        deinit();
        m_pbuffer = p_dummy; 
        return true;
    }

    // Can only pass addresses of named values, most literals aren't allowed! 
    void push_back(const void *p_item) {
        if (m_ncount + 1 > m_ncapacity) {
            resize((m_ncapacity == 0) ? 1 : m_ncapacity * 2);    
        }
        // m_ncount, as is, points to the *next* available spot in the buffer.
        copy_to(get_address_of(m_ncount++), p_item);
    }
    
    void create_at(void *p_dest) {
        m_create(p_dest, m_nsizeof);
    }
    
    void pop_back(void *p_dest) {
        void *p_top = get_address_of(m_ncount - 1);
        copy_to(p_dest, p_top);
        create_at(p_top);     
        m_ncount--;
    }
};

// Cleanup since the macro was already expanded before compilation.
#undef ga_logerror
