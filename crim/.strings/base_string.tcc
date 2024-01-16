#pragma once

#include <cstring>
#include <climits>

#include <algorithm>

#include "memory.tcc"

#define crim_logerror(func, info) \
    crim_logerror_nofunc("crim::base_string<T>", func, info)

namespace crim {
    template<class CharT, class AllocT = allocator<CharT>> class base_string {
    private:
        using size_type = typename AllocT::size_type;
        
        AllocT m_memory; // Abstracts away memory-related business.

        /** 
         * Anonymous unions expose their members to the enclosing scope.
         * But with templates we can't get sizeof one union member properly.
         * Either the union type or the union instance need to be named.
         *  - https://stackoverflow.com/a/28820862
         */
        union {
            struct {CharT *ptr; size_type capacity;} heapbuf;
            CharT stackbuf[sizeof(heapbuf)] = {0};
        } m_data;
        size_type m_ncount = 0; // Current number of non-nul characters.

        // w/o explicit "cast", would get the following error:
        // "enumerated and non-enumerated type in conditional expression"
        // for `capacity()`.
        enum BufInfo : size_type {
            // Number of elements that `stackbuf` can hold: likely 16 `CharT`'s.
            stackbuf_cap = sizeof(m_data.stackbuf) / sizeof(CharT),

            // Last valid index into stackbuf, after this we should use heapbuf.
            stackbuf_end = stackbuf_cap - 1,

            // Heapbuf's initial allocated size: likely 32 `CharT`'s.
            heapbuf_cap = stackbuf_cap * 2
        };

        // Primarily used to access const overloads with non-const `this`. 
        const base_string *const_this()
        {
            return const_cast<const base_string*>(this);
        }
        
    public: 
        base_string(size_type n_length = 0) 
            : m_memory{}
            , m_data{}
            , m_ncount{n_length}
        {}

        // TODO: create my own char_traits  
        base_string(const CharT *p_literal) : base_string(std::strlen(p_literal))
        {
            isshort() ? copy_short(p_literal) : copy_long(p_literal);
        }

        // Copy-constructor, does a deep copy of `other`'s buffer.
        base_string(const base_string &other) : base_string(other.length())
        {
            copy_instance(other);
        }
        
        // Move-constructor. Takes "ownership" of `rvalue`'s pointers, if any.
        base_string(base_string &&rvalue) : base_string(rvalue.length())
        {
            // Named parameters which are rvalue refs "decays" to lvalue ref.
            move_instance(std::move(rvalue)); 
        }
        
        // If we're a short string, we don't need to deallocate anything.
        ~base_string()
        {
            if (isshort()) {
                return;
            }
            for (size_type i = 0; i < m_ncount; i++) {
                m_memory.destroy(&m_data.heapbuf.ptr[i]);
            }
            m_memory.deallocate(m_data.heapbuf.ptr, m_data.heapbuf.capacity);
        }

    private:
        void copy_short(const CharT *p_literal)
        {
            // Add 1 to the count so we also copy the nul char.
            m_memory.copy(m_data.stackbuf, p_literal, m_ncount + 1);
        }
        
        void copy_long(const CharT *p_src)
        {
            // Add 1 to count to also copy nul char from the literal.
            m_data.heapbuf.capacity = m_ncount + 1;
            m_data.heapbuf.ptr = m_memory.allocate(m_data.heapbuf.capacity);
            m_memory.copy(m_data.heapbuf.ptr, p_src, m_data.heapbuf.capacity);
        }

        void copy_instance(const base_string &r_src)
        {
            const CharT *p_data = r_src.data();
            r_src.isshort() ? copy_short(p_data) : copy_long(p_data);
        }
        
        // We "take ownership" of `rvalue`'s heapbuf if it's been allocated.
        // This is so `rvalue`'s pointers aren't freed on destruction.
        void move_instance(base_string &&rvalue)
        {
            if (rvalue.isshort()) {
                copy_short(rvalue.c_str());
                return;
            }
            m_data.heapbuf.capacity = rvalue.capacity();
            m_data.heapbuf.ptr = rvalue.data();
            // Rvalue should now use stackbuf, which is all 0's.
            rvalue.m_ncount = 0;
            rvalue.m_data.heapbuf.ptr = nullptr;
            rvalue.m_data.heapbuf.capacity = 0;
        }

    public: 
        // Read-only element access.
        const CharT &at(size_type n_index) const
        {
            if (n_index >= m_ncount) {
                crim_logerror("at", "Requested an invalid index!");
                throw std::out_of_range("See log message.");
            }
            return isshort() ? m_data.stackbuf[n_index] : m_data.heapbuf.ptr[n_index];
        }
        
        // Read-write element access.
        CharT &at(size_type n_index)
        {
            // We can cast `this` to const and uncast const from const methods'
            // return values because `this` wasn't const to begin with.
            return const_cast<CharT&>(const_this()->at(n_index));
        }
        

        // Check if we're a short string, in which case we're using `stackbuf`.
        // Inside of `push_back`, we use a slightly different check.
        bool isshort() const noexcept
        {
            // Stupid but this should solve the off by 1 error we had,
            // Since m_ncount of 15 OR 16 are still short strings,
            // But if m_ncount == 16 and we do a push_back, we init heapbuf.
            return m_ncount <= BufInfo::stackbuf_cap;
        }
        
        // Number of `CharT`'s in either `stackbuf` or `heapbuf`.
        size_type length() const noexcept
        {
            return m_ncount;
        }
        
        // Includes allocated capacity for the terminating nul character.
        size_type capacity() const noexcept
        {
            return isshort() ? BufInfo::stackbuf_cap : m_data.heapbuf.capacity;
        }
    private:
        // TODO: maybe we can be a bit smart with this and use a local instance.
        bool init_heapbuf()
        {
            // Copy-by-value on the old data first.
            CharT p_prevdata[BufInfo::stackbuf_cap];
            m_memory.copy(p_prevdata, m_data.stackbuf, BufInfo::stackbuf_cap);

            // Careful: now assigning the active union members!
            m_data.heapbuf.capacity = BufInfo::heapbuf_cap;
            m_data.heapbuf.ptr = m_memory.allocate(m_data.heapbuf.capacity);

            // To be handled elsewhere, but do log the error.
            if (m_data.heapbuf.ptr == nullptr) {
                crim_logerror("init_heapbuf", "m_memory.allocate() failed!");
                return false;
            }

            m_memory.copy(m_data.heapbuf.ptr, p_prevdata, m_ncount);
            return true;
        }
    public:
        // TODO: When shortening the buffer, check if we need to use stackbuf
        bool resize(size_type n_newcap) 
        {
            // Don't do anything so we don't waste our time.
            if (n_newcap == m_data.heapbuf.capacity) {
                return true;
            }

            // When shortening the buffer, we want to copy less elements.
            if (n_newcap < m_data.heapbuf.capacity) {
                m_ncount = n_newcap;
            }

            // I do it this way because C++ is hell bent on casting hell.
            size_type n_alloc = sizeof(*m_data.heapbuf.ptr) * n_newcap;
            CharT *p_dummy = m_memory.allocate(n_alloc);
            if (p_dummy == nullptr) {
                crim_logerror("resize", "m_memory.allocate() returned a nullptr!");
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
        // But for heapbuf, we constantly also append a nul char after `ch`.
        bool push_back(CharT ch)
        {
            // Can't use isshort() due to off by 1, since once we need to write
            // to the 16th index we need to start the heapbuf.
            if (m_ncount < BufInfo::stackbuf_cap) {
                m_data.stackbuf[m_ncount++] = ch;
                return true;
            }
            // Initialize heapbuf exactly once, after this `isshort()` == false.
            if (m_ncount == BufInfo::stackbuf_cap) {
                if (init_heapbuf() == false) {
                    crim_logerror("push_back", "init_heapbuf() failed!");
                    return false;
                }
            }
            // Check for enough allocated memory, if not then grow buffer by 2.
            if (m_ncount + 1 >= m_data.heapbuf.capacity) {
                if (resize(m_data.heapbuf.capacity * 2) == false) {
                    crim_logerror("push_back", "resize() failed!");
                    return false;
                }
            }
            // Since heapbuf has uninitialized memory, we also append nul char.
            m_data.heapbuf.ptr[m_ncount++] = ch;
            m_data.heapbuf.ptr[m_ncount] = static_cast<CharT>('\0');
            return true;
        }
        
        const CharT *data() const noexcept
        {
            return isshort() ? m_data.stackbuf : m_data.heapbuf.ptr;
        }
        
        CharT *data() noexcept
        {
            return const_cast<CharT*>(const_this()->data());
        }

        // Return already-nul terminated contents.
        const CharT *c_str() const noexcept
        {
            return data();
        }
    };
}

namespace crim {
    using cstring = base_string<char>;
}

#undef crim_logerror
