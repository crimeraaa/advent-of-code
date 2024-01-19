#pragma once

#include <cstring> /* std::strlen */
#include <algorithm> /* std::move */
#include <stdexcept> /* std::out_of_range */

#include "bitmanip.hpp"
#include "memory.tcc"
#include "type_traits.tcc"
#include "utility.tcc"

#define crim_logerror(func, info) \
    crim_logerror_nofunc("crim::base_string<T>", func, info)

namespace crim {
    template<typename CharT, class Traits, class Allocator> 
    class base_string;
}

/**
 * BEGIN BASE_STRING IMPLEMENTATION -*------------------------------------------
*/

/**
 * @brief   A simple reimplementation of `std::string`.
 *          Also with short string optimization to reduce heap-allocations!
 *
 * @note    Default values must go in forward declarations.
 */
template<typename CharT, class Traits, class Allocator> 
class crim::base_string {
/**
 * BEGIN TYPEDEFS -*------------------------------------------------------------
 */
private:
    using value_type = CharT;
    using traits_type = Traits;
    using allocator_type = Allocator;

    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = typename Allocator::size_type;
/** 
 * END TYPEDEFS -*--------------------------------------------------------------
 */

/**
 * BEGIN DATA MEMBERS -*--------------------------------------------------------
 */
private:
    allocator_type m_allocator;
    /** 
     * Anonymous unions expose their members to the enclosing scope.
     * But with templates we can't get sizeof one union member properly.
     * Either the union type or the union instance need to be named.
     *  - https://stackoverflow.com/a/28820862
     */
    union databuf {
        struct heapbuf {
            pointer ptr; 
            size_type cap;
        } heap;
        value_type stack[sizeof(heap)] = {0};
    } m_data;
    size_type m_ncount{0}; 

    // w/o explicit "cast", would get the following error:
    // "enumerated and non-enumerated type in conditional expression"
    // for `capacity()`.
    enum limit : size_type {
        stack_max_cap = sizeof(m_data.stack) / sizeof(value_type),
        heap_init_cap = stack_max_cap * 2
    };

    // Primarily used to access const overloads with non-const `this`. 
    const base_string *const_this()
    {
        return const_cast<const base_string*>(this);
    }
/**
 * END DATA MEMBERS -*----------------------------------------------------------
 */

/**
 * BEGIN CONSTRUCTOR/DESTRUCTORS -*---------------------------------------------
 */
public: 
    base_string(size_type n_length = 0) noexcept
        : m_allocator{}
        , m_data{}
        , m_ncount{n_length}
    {}

    base_string(const_pointer p_literal) 
        : base_string(traits_type::length(p_literal))
    {
        copy_string(p_literal);
    }

    base_string(const base_string &other) 
        : base_string(other.c_str())
    {}
    
    base_string(base_string &&other) 
        : base_string(other.length())
    {
        // Named parameters which are rvalue refs "decays" to lvalue ref.
        move_instance(crim::rvalue_cast(other)); 
    }
    
    ~base_string()
    {
        // If we're a short string, we don't need to deallocate anything.
        if (isshort()) {
            return;
        }
        // Non-trivially destructible types: need to destroy them all first.
        m_allocator.deallocate(m_data.heap.ptr, m_data.heap.cap);
    }

private:
    /**
     * @brief   Call `Traits::copy` on the correct buffer. We add 1 to the count
     *          so that we also copy the nul character at the end.
     */
    void copy_string(const_pointer p_data)
    {
        if (isshort()) {
            traits_type::copy(m_data.stack, p_data, m_ncount + 1);
        } else {
            // For long strings, we only start allocations by powers of 2.
            m_data.heap.cap = crim::bit::next_power(m_ncount + 1);
            m_data.heap.ptr = m_allocator.allocate(m_data.heap.cap);
            traits_type::copy(m_data.heap.ptr, p_data, m_data.heap.cap);
        }
    }
    
    /**
     * @brief   We "take ownership" of `other`'s `m_data.heap.ptr`. We only do
     *          do so when we know it's been allocated for. Otherwise, we can
     *          just copy by value on `m_data.stack`.
     *
     * @note    This is important to ensure that, in case `other` is indeed a 
     *          heap-allocated string, it's pointer doesn't get freed when its
     *          destructor is called.
     */
    void move_instance(base_string &&other)
    {
        if (other.isshort()) {
            traits_type::copy(m_data.stack, other.c_str(), m_ncount + 1);
            return;
        }
        m_data.heap.cap = other.capacity();
        m_data.heap.ptr = other.data();
        // other should now use stack, which is all 0's.
        other.m_ncount = 0;
        other.m_data.heap.ptr = nullptr;
        other.m_data.heap.cap = 0;
    }

public: 
    /**
     * @brief       Read-only element access.
     * 
     * @exception   `std::out_of_range` if `n_index` if out of bounds.
    */
    const_reference at(size_type n_index) const
    {
        if (n_index >= m_ncount) {
            crim_logerror("at", "Requested an invalid index!");
            throw std::out_of_range("See log message.");
        }
        return isshort() ? m_data.stack[n_index] : m_data.heap.ptr[n_index];
    }
    
    /**
     * @brief       Read-write element access.
     * 
     * @exception   `std::out_of_range` if `n_index` if out of bounds.
    */
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
        return m_ncount <= stack_max_cap;
    }
    
    // Count of non-nul `CharT`'s we're currently storing.
    size_type length() const noexcept
    {
        return m_ncount;
    }
    
    // Includes allocated capacity for the terminating nul character.
    size_type capacity() const noexcept
    {
        return isshort() ? stack_max_cap : m_data.heap.cap;
    }
private:
    /**
     * @brief   Valgrind shows that a local instance uses a little bit more
     *          memory than an array of known size. Hmm...
     */
    bool init_heap()
    {
        // Copy-by-value on the old data first.
        value_type p_prevdata[stack_max_cap];
        traits_type::copy(p_prevdata, m_data.stack, stack_max_cap);

        // Careful: now assigning the active union members!
        m_data.heap.cap = heap_init_cap;
        m_data.heap.ptr = m_allocator.allocate(m_data.heap.cap);

        // To be handled elsewhere, but do log the error.
        if (m_data.heap.ptr == nullptr) {
            crim_logerror("init_heap", "m_allocator.allocate() failed!");
            return false;
        }
        traits_type::copy(m_data.heap.ptr, p_prevdata, m_ncount);
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
        pointer p_dummy = m_allocator.allocate(n_newcap);
        if (p_dummy == nullptr) {
            crim_logerror("resize", "m_allocator.allocate() returned a nullptr!");
            return false;
        }
        traits_type::copy(p_dummy, m_data.heap.ptr, m_ncount);

        // Switcharoo and cleanup.
        p_dummy[m_ncount] = '\0';
        m_allocator.deallocate(m_data.heap.ptr, m_data.heap.cap);
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
        if (m_ncount < stack_max_cap) {
            m_data.stack[m_ncount++] = ch;
            return true;
        }
        // Initialize `m_data.heap` only once, after this `isshort() == false`.
        if (m_ncount == stack_max_cap) {
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
        m_data.heap.ptr[m_ncount++] = ch;
        m_data.heap.ptr[m_ncount] = '\0';
        return true;
    }
    
    const_pointer data() const noexcept
    {
        return isshort() ? m_data.stack : m_data.heap.ptr;
    }
    
    pointer data() noexcept
    {
        return const_cast<pointer>(const_this()->data());
    }

    // Return already-nul terminated contents.
    const_pointer c_str() const noexcept
    {
        return data();
    }
};

/**
 * END BASE STRING IMPLEMENTATION -*--------------------------------------------
 */

/**
 * BEGIN BASE STRING INSTANTIATIONS -*------------------------------------------
 */

namespace crim {
    using cstring = base_string<char, char_traits<char>, allocator<char>>;
    using wcstring = base_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>>;
}

/**
 * END BASE STRING INSTANTIATIONS -*--------------------------------------------
 */

#undef crim_logerror
