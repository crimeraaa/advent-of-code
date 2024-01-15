#pragma once

#include <cstring>
#include <climits>

#include <algorithm>

#include "allocator.tcc"

#define crim_base_string_logerror(info) crim_logerror("base_string<T>", info)

namespace crim {
    template<class CharT, class AllocT = allocator<CharT>> class base_string {
    private:
        AllocT m_memory; // Abstracts away memory-related business.

        // Anonymous unions expose their members to the enclosing scope.
        // But with templates we can't get sizeof one union member properly.
        // Either the union type or the union need to be named.
        //  - https://stackoverflow.com/a/28820862
        union {
            struct {CharT *ptr; size_t capacity;} heapbuf;
            CharT stackbuf[sizeof(heapbuf)] = {0};
        } m_data;
        size_t m_ncount = 0; // Current number of non-nul characters.

        enum BufInfo {
            stackbuf_cap = sizeof(m_data.stackbuf),
            stackbuf_end = stackbuf_cap - 1,
            neapbuf_start = stackbuf_cap,
            heapbuf_cap = stackbuf_cap * 2
        };

        // Primarily used to access const overloads with non-const `this`. 
        const base_string *const_this() {
            return const_cast<const base_string*>(this);
        }
        
    public: 
        base_string(size_t n_length = 0) 
        : m_memory{}
        , m_data{}
        , m_ncount{n_length} {}

        // TODO: create my own char_traits  
        base_string(const CharT *p_literal) 
        : base_string(std::strlen(p_literal)) {
            isshort() ? copy_short(p_literal) : copy_long(p_literal);
        }

        // Copy-constructor, does a deep copy of `other`'s buffer.
        base_string(const base_string &other) 
        : base_string(other.length()) {
            copy_instance(other);
        }
        
        // Move-constructor. Takes "ownership" of `rvalue`'s pointers, if any.
        base_string(base_string &&rvalue) 
        : base_string(rvalue.length()) {
            move_instance(std::move(rvalue)); // "decays" to lvalue ref otherwise
        }
        
        // If we're a short string, we don't need to deallocate anything.
        ~base_string() {
            if (isshort()) {
                return;
            }
            for (size_t i = 0; i < m_ncount; i++) {
                m_memory.destroy_at(&m_data.heapbuf.ptr[i]);
            }
            m_memory.deallocate(m_data.heapbuf.ptr, m_data.heapbuf.capacity);
        }

    private:
        void copy_short(const CharT *p_literal) {
            // Add 1 to the count so we also copy the nul char.
            m_memory.copy(m_data.stackbuf, p_literal, m_ncount + 1);
        }
        
        void copy_long(const CharT *p_src) {
            m_data.heapbuf.capacity = m_ncount + 1;
            m_data.heapbuf.ptr = m_memory.allocate(m_data.heapbuf.capacity);
            m_memory.copy(m_data.heapbuf.ptr, p_src, m_data.heapbuf.capacity);
        }

        void copy_instance(const base_string &r_src) {
            const CharT *p_data = r_src.c_str();
            r_src.isshort() ? copy_short(p_data) : copy_long(p_data);
        }
        
        // We "take ownership" of `rvalue`'s heapbuf if it's been allocated.
        // This is so `rvalue`'s pointers aren't freed on destruction.
        void move_instance(base_string &&rvalue) {
            if (rvalue.isshort()) {
                copy_short(rvalue.c_str());
                return;
            }
            m_data.heapbuf.capacity = rvalue.capacity();
            m_data.heapbuf.ptr = rvalue.m_data.heapbuf.ptr;
            // This should cause rvalue to now use stackbuf.
            rvalue.m_ncount = 0;
            rvalue.m_data.heapbuf.ptr = nullptr;
            rvalue.m_data.heapbuf.capacity = 0;
        }

    public: 
        const CharT &at(size_t n_index) const {
            if (n_index >= m_ncount) {
                crim_base_string_logerror("Requested an invalid index!");
                throw std::out_of_range("See log message.");
            }
            return isshort() ? m_data.stackbuf[n_index] : m_data.heapbuf.ptr[n_index];
        }

        // We can cast `this` to const and uncast const from const methods'
        // return values because they weren't const to begin with.
        CharT &at(size_t n_index) {
            return const_cast<CharT&>(const_this()->at(n_index));
        }

        // Stupid but this should solve the off by 1 error we had,
        // Since m_ncount of 15 OR 16 are still short strings.
        // Inside of `push_back`, we use a slightly different check.
        bool isshort() const {
            return m_ncount <= BufInfo::stackbuf_cap;
        }
        
        size_t length() const {
            return m_ncount;
        }
        
        // Includes allocated capacity for the terminating nul character.
        size_t capacity() const {
            return isshort() ? sizeof(m_data.stackbuf) : m_data.heapbuf.capacity;
        }
    private:
        // TODO: maybe we can be a bit smart with this and use a local instance.
        bool init_heapbuf() {
            // Copy-by-value on the old data first.
            CharT p_prevdata[sizeof(m_data.stackbuf)];
            m_memory.copy(p_prevdata, m_data.stackbuf, m_ncount);

            // Careful: now assigning the active union members!
            m_data.heapbuf.capacity = BufInfo::heapbuf_cap;
            size_t n_alloc = sizeof(*p_prevdata) * m_data.heapbuf.capacity;
            m_data.heapbuf.ptr = m_memory.allocate(n_alloc);

            // To be handled elsewhere, but do log the error.
            if (m_data.heapbuf.ptr == nullptr) {
                crim_base_string_logerror("m_memory.allocate() failed!");
                return false;
            }

            m_memory.copy(m_data.heapbuf.ptr, p_prevdata, m_ncount);
            return true;
        }
    public:
        bool resize(size_t n_newcap) {
            // Don't do anything so we don't waste our time.
            if (n_newcap == m_data.heapbuf.capacity) {
                return true;
            }

            // Shortening the buffer has a few quirks.
            if (n_newcap < m_data.heapbuf.capacity) {
                m_ncount = n_newcap;
            }

            // I do it this way because C++ is hell bent on casting hell.
            size_t n_alloc = sizeof(*m_data.heapbuf.ptr) * n_newcap;
            CharT *p_dummy = m_memory.allocate(n_alloc);
            if (p_dummy == nullptr) {
                crim_base_string_logerror("m_memory.allocate() returned a nullptr!");
                return false;
            }
            m_memory.copy(p_dummy, m_data.heapbuf.ptr, m_ncount);

            // Switcharoo and cleanup.
            p_dummy[m_ncount] = '\0';
            m_memory.deallocate(m_data.heapbuf.ptr, m_data.heapbuf.capacity);
            m_data.heapbuf.ptr = p_dummy;
            m_data.heapbuf.capacity = n_newcap;
            return true;            
        }

        // For stackbuf, all the data is 0-initialized so we just append `ch`.
        // But for heapbuf, we constantly also append a nul char.
        bool push_back(CharT ch) {
            // Can't use isshort() due to off by 1, since once we need to write
            // to the 16th index we need to start the heapbuf.
            if (m_ncount < BufInfo::stackbuf_cap) {
                m_data.stackbuf[m_ncount++] = ch;
                return true;
            }

            // Initialize heapbuf exactly once.
            if (m_ncount == BufInfo::stackbuf_cap) {
                if (init_heapbuf() == false) {
                    crim_base_string_logerror("init_heapbuf() failed!");
                    return false;
                }
            }

            // Check for enough allocated memory, if not then try to resize!
            if (m_ncount + 1 >= m_data.heapbuf.capacity) {
                if (resize(m_data.heapbuf.capacity * 2) == false) {
                    crim_base_string_logerror("resize() failed!");
                    return false;
                }
            }
            m_data.heapbuf.ptr[m_ncount++] = ch;
            return true;
        }
        
        // I'm shocked that this compiles since we modify the heapbuf pointer...
        const CharT *c_str() const noexcept {
            // Stackbuf is 0-initialized, so we don't need to append anything.
            if (isshort()) {
                return m_data.stackbuf;
            }
            // For heapbuf we most likely have to append nul ourselves.
            m_data.heapbuf.ptr[m_ncount] = static_cast<CharT>('\0');
            return m_data.heapbuf.ptr;
        }
        
        const CharT *data() const noexcept {
            return c_str();
        }
        
        CharT *data() noexcept {
            return const_cast<CharT*>(const_this()->c_str());
        }
    };
}

namespace crim {
    using cstring = base_string<char>;
}

#undef crim_base_string_logerror
