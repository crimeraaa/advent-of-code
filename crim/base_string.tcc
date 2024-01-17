#pragma once

#include <cstring> /* std::strlen */
#include <algorithm> /* std::move */
#include <stdexcept> /* std::out_of_range */

#include "memory.tcc"
#include "bitmanip.hpp"

#define crim_logerror(func, info) \
    crim_logerror_nofunc("crim::base_string<T>", func, info)

namespace crim {
    template<class CharT, class AllocT = allocator<CharT>> 
    class base_string;
}

/**
 * @brief   A simple reimplementation of `std::string`.
 *          Also with short string optimization to reduce heap-allocations!
 *
 * @note    Default values must go in forward declarations.
 */
template<class CharT, class AllocT> 
class crim::base_string {
private:
    using value_type = typename AllocT::value_type;
    using size_type = typename AllocT::size_type;
    
    AllocT m_memory; 

    /** 
     * Anonymous unions expose their members to the enclosing scope.
     * But with templates we can't get sizeof one union member properly.
     * Either the union type or the union instance need to be named.
     *  - https://stackoverflow.com/a/28820862
     */
    union databuf {
        struct heapbuf {
            CharT *ptr; 
            size_type cap;
        } heap;
        CharT stack[sizeof(heap)] = {0};
    } m_data;
    size_type m_ncount{0}; 

    // w/o explicit "cast", would get the following error:
    // "enumerated and non-enumerated type in conditional expression"
    // for `capacity()`.
    enum limit : size_type {
        stack_max_cap = sizeof(m_data.stack) / sizeof(CharT),
        heap_init_cap = stack_max_cap * 2
    };

    // Primarily used to access const overloads with non-const `this`. 
    const base_string *const_this()
    {
        return const_cast<const base_string*>(this);
    }
    
public: 
    base_string(size_type n_length = 0) noexcept
        : m_memory{}
        , m_data{}
        , m_ncount{n_length}
    {}

    base_string(const CharT *p_literal) 
        : base_string(std::strlen(p_literal))
    {
        // TODO: create my own char_traits  
        copy_c_string(p_literal);
    }

    base_string(const base_string &other) 
        : base_string(other.c_str())
    {}
    
    base_string(base_string &&rvalue) 
        : base_string(rvalue.length())
    {
        // Named parameters which are rvalue refs "decays" to lvalue ref.
        move_instance(std::move(rvalue)); 
    }
    
    ~base_string()
    {
        // If we're a short string, we don't need to deallocate anything.
        if (isshort()) {
            return;
        }
        for (size_type i = 0; i < m_ncount; i++) {
            m_memory.destroy(&m_data.heap.ptr[i]);
        }
        m_memory.deallocate(m_data.heap.ptr, m_data.heap.cap);
    }

private:
    void copy_short(const CharT *p_literal)
    {
        // Add 1 to the count so we also copy the nul char.
        m_memory.uninitialized_copy(m_data.stack, p_literal, m_ncount + 1);
    }
    
    void copy_long(const CharT *p_src)
    {
        // Add 1 to count to also copy nul char from the literal.
        // For long strings, we only start allocations by powers of 2.
        m_data.heap.cap = {crim::digits::bit_next_power(m_ncount + 1)};
        m_data.heap.ptr = m_memory.allocate(m_data.heap.cap);
        m_memory.uninitialized_copy(m_data.heap.ptr, p_src, m_data.heap.cap);
    }

    void copy_c_string(const CharT *p_data)
    {
        // For copy-ctor, since we copied other's length this should be OK.
        isshort() ? copy_short(p_data) : copy_long(p_data);
    }
    
    // We "take ownership" of `rvalue`'s m_data.heap.ptr if it's been allocated.
    // This is so `rvalue`'s pointers aren't freed on destruction.
    void move_instance(base_string &&rvalue)
    {
        if (rvalue.isshort()) {
            copy_short(rvalue.c_str());
            return;
        }
        m_data.heap.cap = rvalue.capacity();
        m_data.heap.ptr = rvalue.data();
        // rvalue should now use stack, which is all 0's.
        rvalue.m_ncount = 0;
        rvalue.m_data.heap.ptr = nullptr;
        rvalue.m_data.heap.cap = 0;
    }

public: 
    // Read-only element access.
    const CharT &at(size_type n_index) const
    {
        if (n_index >= m_ncount) {
            crim_logerror("at", "Requested an invalid index!");
            throw std::out_of_range("See log message.");
        }
        return isshort() ? m_data.stack[n_index] : m_data.heap.ptr[n_index];
    }
    
    // Read-write element access.
    CharT &at(size_type n_index)
    {
        // We can cast `this` to const and uncast const from const methods'
        // return values because `this` wasn't const to begin with.
        return const_cast<CharT&>(const_this()->at(n_index));
    }
    

    /**
     * @brief   Check if we're still a short string. In that case we're still 
     *          using the `stack` union array.
     *          Inside of `push_back()`, we use a slightly different check.
     * 
     * @note    `m_ncount` of 15 and/or 16 are still short strings.
     *          But if `m_ncount == 16` and then we do a push_back, we need to 
     *          init `m_data.heap`. 
     */
    bool isshort() const noexcept
    {
        return m_ncount <= limit::stack_max_cap;
    }
    
    // Count of non-nul `CharT`'s we're currently storing.
    size_type length() const noexcept
    {
        return m_ncount;
    }
    
    // Includes allocated capacity for the terminating nul character.
    size_type capacity() const noexcept
    {
        return isshort() ? limit::stack_max_cap : m_data.heap.cap;
    }
private:
    /**
     * @brief   Valgrind shows that a local instance uses a little bit more
     *          memory than an array of known size. Hmm...
     */
    bool init_heap()
    {
        // Copy-by-value on the old data first.
        CharT p_prevdata[limit::stack_max_cap];
        m_memory.uninitialized_copy(p_prevdata, m_data.stack, limit::stack_max_cap);

        // Careful: now assigning the active union members!
        m_data.heap.cap = limit::heap_init_cap;
        m_data.heap.ptr = m_memory.allocate(m_data.heap.cap);

        // To be handled elsewhere, but do log the error.
        if (m_data.heap.ptr == nullptr) {
            crim_logerror("init_heap", "m_memory.allocate() failed!");
            return false;
        }

        m_memory.uninitialized_copy(m_data.heap.ptr, p_prevdata, m_ncount);
        return true;
    }
public:
    // TODO: When shortening the buffer, check if we need to use stack
    bool resize(size_type n_newcap) 
    {
        // Don't do anything so we don't waste our time.
        if (n_newcap == m_data.heap.cap) {
            return true;
        }

        // When shortening the buffer, we want to copy less elements.
        if (n_newcap < m_data.heap.cap) {
            m_ncount = n_newcap;
        }

        // I do it this way because C++ is hell bent on casting hell.
        size_type n_alloc{sizeof(*m_data.heap.ptr) * n_newcap};
        CharT *p_dummy = m_memory.allocate(n_alloc);
        if (p_dummy == nullptr) {
            crim_logerror("resize", "m_memory.allocate() returned a nullptr!");
            return false;
        }
        m_memory.uninitialized_copy(p_dummy, m_data.heap.ptr, m_ncount);

        // Switcharoo and cleanup.
        p_dummy[m_ncount] = '\0';
        m_memory.deallocate(m_data.heap.ptr, m_data.heap.cap);
        m_data.heap.ptr = p_dummy;
        m_data.heap.cap = n_newcap;
        return true;            
    }

    // For stack, all the data is 0-initialized so we just append `ch`.
    // But for `m_data.hea`p, we constantly also append a nul char after `ch`.
    bool push_back(CharT ch)
    {
        // Can't use `isshort()` due to off by 1, since once we need to write
        // to the 16th index we also need to start `m_data.heap.ptr`.
        if (m_ncount < limit::stack_max_cap) {
            m_data.stack[m_ncount++] = ch;
            return true;
        }
        // Initialize `m_data.heap` only once, after this `isshort() == false`.
        if (m_ncount == limit::stack_max_cap) {
            if (init_heap() == false) {
                crim_logerror("push_back", "init_heap() failed!");
                return false;
            }
        }
        // Check for enough allocated memory, if not then grow buffer by 2.
        if (m_ncount + 1 >= m_data.heap.cap) {
            if (resize(m_data.heap.cap * 2) == false) {
                crim_logerror("push_back", "resize() failed!");
                return false;
            }
        }
        // `m_data.heap.ptr` has uninitialized memory, need to append nul char.
        m_memory.construct(&m_data.heap.ptr[m_ncount++], ch);
        m_memory.construct(&m_data.heap.ptr[m_ncount], '\0');
        return true;
    }
    
    const CharT *data() const noexcept
    {
        return isshort() ? m_data.stack : m_data.heap.ptr;
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

namespace crim {
    using cstring = base_string<char>;
}

#undef crim_logerror
