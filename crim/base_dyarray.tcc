#pragma once

/*- -*- ---***--- -*-        THE C STANDARD LIBRARY       -*- ---***---  -*- -*/
#include <stddef.h>
#include <string.h>

/*- -*- ---***--- -*-       THE C++ STANDARD LIBRARY      -*- ---***---  -*- -*/
#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <functional>
/*- -*- ---***--- -*-                                     -*- ---***---  -*- -*/

// Classes that inherit from parents can be forward declared, but the 
// inheritance itself must only go in the class definition proper.
namespace crim {
    template<class ElemT> struct iterator;

    template<
        class DerivedT, 
        typename ElemT, 
        class AllocT = std::allocator<ElemT>
    > class base_dyarray;
};

/**
 * @brief       An iterator class for use with range-based for loops.
 * 
 * @details     You'll have to provide `begin()` and `end()` methods yourself.
 * 
 *              Such functions must return pointers to the template type.
 *              For help, see my dyarray base class implementation for an example.
 * 
 * @tparam      ElemT   Desired type of the caller template's buffer.
 * 
 * @note        Valgrind suggests that using an out of bounds memory address
 *              for `m_end` is a bad idea, so be careful how you update it!
 *              Instead of dereferencing an index, use pointer arithmetic.
 */
template<class ElemT> struct crim::iterator {
    ElemT *m_begin; // Address of first written element.
    ElemT *m_end; // Address right past the last written element.

    iterator(ElemT *start, ElemT *stop) : m_begin{start}, m_end{stop} {}

    iterator(ElemT *start, size_t count) : iterator(start, start + count) {}

    iterator(iterator &src) : iterator(src.begin(), src.end()) {}

    iterator(iterator &&src) : iterator(src.begin(), src.end()) {}

    void set_pointers(ElemT *start, ElemT *stop) {
        m_begin = start;
        m_end = stop;
    }

    void set_range(ElemT *start, size_t count = 0) {
        set_pointers(start, start + count);
    }

    static inline iterator &copy(iterator &lhs, const iterator &rhs) {
        lhs.m_begin = rhs.m_begin;
        lhs.m_end = rhs.m_end;
        return lhs;
    }

    // Copy-assignment.
    iterator &operator=(const iterator &src) {
        return copy(*this, src);
    }

    /**
     * @brief   Move-assignment.
     * 
     * @note    `src`, being named, when passed to other functions, is treated
     *          as if it were an lvalue reference (a.k.a. `const iterator &`).
     */
    iterator &operator=(const iterator &&src) {
        return copy(*this, src);
    }

    /* -*- ---***--- -*- ITERATOR POINTER ACCESS METHODS -*- ---***--- -*- */

    ElemT *begin() { return m_begin; }
    ElemT *end()   { return m_end;   }

    const ElemT *begin() const { return m_begin; }
    const ElemT *end()   const { return m_end;   }

    bool empty() const { return m_begin == m_end; }
};

/**
 * @brief   A somewhat generic growable array that can be templated over.
 * 
 *          This is not meant for you to use in your programs, rather it is
 *          meant to be used by templates to inherit basic functionality to
 *          allow method chaining from child classes.
 * 
 *          Most likely you'll be using this with other templates.
 *          You'll need to pass the templated name, e.g:
 * 
 *          `template<CharT>class string : base_dyarray<string<CharT>, CharT>`
 * 
 * @details This utilizes the Continuous Recurring Template Pattern (CRTP).
 * 
 *          Imagine it like a template class for template classes, where 
 *          descendent templates inherit & specialize from this. At the same
 *          time, they get access to method chaining.
 * 
 *          Although it can get ugly and verbose, this allows us to reuse 
 *          implementation and method chaining of child classes!
 * 
 *          For more information, see: https://stackoverflow.com/a/56423659
 * 
 * @tparam  DerivedT    The derived class's name, usually the templated name.
 * @tparam  ElemT       Buffer's element type.
 */
template<
    class DerivedT, 
    typename ElemT, 
    class AllocT
> class crim::base_dyarray {
private:
    friend DerivedT; // allows access private and protected members of caller!

    DerivedT &derived_cast() {
        return static_cast<DerivedT &>(*this);
    }

    DerivedT &derived_cast() const {
        return static_cast<DerivedT const&>(*this);
    }

    using Malloc = std::allocator_traits<AllocT>;

protected:
    static constexpr size_t START_CAPACITY = 16;
    static constexpr size_t MAX_LENGTH = 0xFFFFFFFF; // 1-based.
    AllocT m_malloc;
    size_t m_length; // Number of elements written to buffer currently.
    size_t m_capacity; // Number of elements that buffer could hold.
    ElemT *m_buffer; // Heap-allocated 1D array of the specified type.
    iterator<ElemT> m_iterator;

    // -------------------- CONSTRUCTORS/DESTRUCTORS ---------------------------
    // NOTICE: These are protected to avoid users calling these by themselves!
    // See: https://www.reddit.com/r/cpp/comments/lhvkzs/comment/gn3nmsx/

    
    base_dyarray(size_t n_len, size_t n_cap, ElemT *p_mem)
    : m_malloc{}
    , m_length{n_len}
    , m_capacity{n_cap}
    , m_buffer{p_mem}
    , m_iterator(m_buffer, m_length)
    {}

    base_dyarray() : base_dyarray(0, 0, nullptr) {}
    
    base_dyarray(size_t n_length, size_t n_capacity) 
    : base_dyarray(n_length, n_capacity, Malloc::allocate(m_malloc, n_capacity))
    {}

    /**
     * @brief   Constructor for "array literals" (curly brace literals) which
     *          are really implemented via `std::initializer_list` objects.
     * 
     *          This copies all the elements of the list into our buffer.
     * 
     *          Upon an append/push_back, this will likely cause a realloc.
     */
    base_dyarray(std::initializer_list<ElemT> list) 
    : base_dyarray(list.size(), list.size()) 
    {
        std::copy(list.begin(), list.end(), begin());
    }

    /**
     * @brief   Copy-constructor, does deep-copy of all of `src`'s data.
     */
    base_dyarray(const base_dyarray &src) 
    : base_dyarray(src.length(), src.capacity()) 
    {
        // Make sure both `src` and `*this`'s iterators are valid!
        std::copy(src.begin(), src.end(), begin());
    }

    /**
     * @brief   Move-constructor, does a shallow copy of `tmp`, which is a
     *          temporary value/expression (usually an rvalue).
     * 
     *          `tmp` itself has its internal buffer set to `nullptr` after 
     *          copying so that upon its destruction, the memory isn't freed!
     * 
     * @note    [See here for help.](https://learn.microsoft.com/en-us/cpp/cpp/move-constructors-and-move-assignment-operators-cpp?view=msvc-170)
     */
    base_dyarray(base_dyarray &&tmp) 
    : base_dyarray(tmp.length(), tmp.capacity(), tmp.begin())
    {
        // erase so that when tmp is destroyed we don't free the memory.
        tmp.m_length = 0;
        tmp.m_capacity = 0;
        tmp.m_buffer = nullptr;
        tmp.m_iterator.set_range(nullptr, 0);
    }

    ~base_dyarray() {
        destroy_buffer();
    }

    void destroy_buffer() {
        // Malloc::deallocate doesn't accept nullptr.
        if (m_buffer == nullptr) {
            return;
        }
        // Contents may be non-trivially destructible so explicitly destroy 'em!
        for (size_t i = 0; i < m_capacity; i++) {
            Malloc::destroy(m_malloc, m_buffer + i);
        }
        // After contents are destroyed then we can get rid of the pointer.
        Malloc::deallocate(m_malloc, m_buffer, m_capacity);
    }

public:
    /* -*- ---***--- -*-   NON-CONST DATA ACCESS METHODS    -*- ---***--- -*- */

    /**
     * @brief   Read-write element access from the internal buffer.
     * 
     * @exception   `std::out_of_range` if requested index is out of bounds.
     *              Should also work for negative indexes as they'll overflow.
     */
    ElemT &at(size_t index) {
        if (index > m_length) {
            throw std::out_of_range("Requested base_dyarray index is invalid!");
        }
        return m_buffer[index];
    }

    /**
     * @brief   Read-write element access from the internal buffer.
     * 
     * @exception   `std::out_of_range` if requested index is out of bounds.
     *              Should also work for negative indexes as they'll overflow.
     */
    ElemT &operator[](size_t index) {
        return at(index);
    }

    /**
     * @brief   Read-write pointer to the start of the internal buffer.
     * 
     *          For strings, this should usually be nul-terminated.
     */
    ElemT *data() {
        return m_buffer;
    }

    /* -*- ---***--- -*-     CONST DATA ACCESS METHODS      -*- ---***--- -*- */

    /** 
     * @brief   See if begin and end iterators point to the exact same address. 
     * 
     *          This is because on empty initialization, they do! But once at 
     *          least one element is pushed back, the end pointer increments.
    */
    bool empty() const {
        return m_iterator.empty();
    }

    /**
     * @brief   Gets number of elements written to the internal buffer. For
     *          strings, this count (likely) already excludes nul-termination.
     */
    size_t length() const {
        return m_length;
    }

    /** 
     * @brief   Gets number of elements our buffer can hold at most currently. 
     *          In other words, this is how much space is allocated for the 
     *          buffer right now.
    */
    size_t capacity() const {
        return m_capacity;
    }

    /**
     * @brief   Maximum allowable elements the buffer can be allocated for.
     * 
     * @note    static constexpr member functions cannot be const functions.
     */
    static constexpr size_t max_length() 
    {
        return MAX_LENGTH;
    }

    /**
     * @brief   Read-only pointer to the internal buffer. Useful for strings.
     *          For string classes, this should return nul terminated data.
     * 
     * @warning If you attempt to modify the buffer from here, good luck!
     * 
     * @note    See [cppreference.com.](https://en.cppreference.com/w/cpp/container/vector/data)
     */
    const ElemT *data() const {
        return m_buffer;
    }

    /* -*- ---***--- -*-              ITERATORS             -*- ---***--- -*- */

    ElemT *begin() {
        return m_iterator.begin();
    }

    ElemT *end() {
        return m_iterator.end();
    }
    
    const ElemT *begin() const {
        return m_iterator.begin();
    }

    const ElemT *end() const {
        return m_iterator.end();
    }

    /* -*- ---***--- -*-      DATA COPYING AND MOVEMENT     -*- ---***--- -*- */
    // Assignment operator overloads cannot be called from the child directly. 
    // You'll need a wrapper like `crim::string &operator=(crim::string &&src)`.

    DerivedT &copy(const base_dyarray &src) {
        // Don't copy ourselves, copying overlapping memory won't end well.
        if (this != &src) {
            destroy_buffer();

            m_buffer = Malloc::allocate(m_malloc, src.capacity());
            // Copy only up to last written index to avoid unitialized memory.
            // std::copy calls copy-constructor for each non-trivial entity.
            std::copy(src.begin(), src.end(), m_buffer);
            m_length = src.m_length;
            m_capacity = src.m_capacity;
            m_iterator.set_range(m_buffer, m_length);
        }
        return derived_cast();
    }

    DerivedT &move(base_dyarray &&src) {
        // If we try to move ourselves, we'll destroy the same buffer!
        if (this != &src) {
            destroy_buffer();

            // Do a shallow copy, assuming `src.m_buffer` is also allocated.
            m_buffer = src.m_buffer;
            m_length = src.m_length;
            m_capacity = src.m_capacity;
            m_iterator = src.m_iterator;

            // Make null so our memory won't be freed just yet.
            src.m_length = 0;
            src.m_capacity = 0;
            src.m_buffer = nullptr;
            src.m_iterator.set_range(nullptr);
        }
        return derived_cast();
    }

public:
    /* -*- ---***--- -*-        BUFFER MANIPULATION         -*- ---***--- -*- */

    /**
     * @brief   Moves `entry` to the top of the internal buffer. That is, it is 
     *          move-assigned to the index after the previously written element. 
     * 
     *          It's mainly helpful for objects with a clear move-assigment,
     *          such as by overloading the `=` operator. For example, see:
     *          `crim::dystring &operator=(crim::dystring &&)`.
     * 
     * @note    We take an rvalue reference so `entry` itself maybe invalidated!
     *          This function only really works with temporary values. 
     */
    DerivedT &push_back(ElemT &&entry) {
        check_size();
        // `entry`, being named, "decays" to an lvalue reference
        m_buffer[m_length++] = std::move(entry);
        return derived_cast();
    }

    /**
     * @brief   Copies `entry` by value to the top of the internal buffer. That 
     *          is, its copy-constructor (or a default one) is called to assign
     *          that value to the index after the previously written element.
     * 
     * @note    We take a const lvalue reference so that we do not need to
     *          overload for const and non-const, as we do not intend to modify
     *          `entry` in any way.
     */
    DerivedT &push_back(const ElemT &entry) {
        check_size();
        m_buffer[m_length++] = entry;
        return derived_cast();
    }

private:
    void check_size() {
        // Also helpful to indicate when I accidentally overflow
        if (m_length > MAX_LENGTH) {
            throw std::length_error("Reached crim::base_dyarray::MAXLENGTH!");
        } else if (m_length + 1 > m_capacity) {
            // All the cool kids seem to grow their buffers by doubling it!
            resize((m_capacity == 0) ? 16 : m_capacity * 2); 
        }
        // Don't try to dereference unitialized memory and get its address!
        m_iterator.m_end++;
    }

public:
    /**
     * @brief   Reallocates memory for the buffer by extension or narrowing.
     * 
     * @details Similar to C's `realloc`. Cleans up the old memory as well.
     *          Updates internals for you accordingly so you don't have to!
     * 
     * @param   new_size    New requested buffer size, may be greater or lesser.
     * 
     * @warning For strings, this may not guarantee nul termination!
     */
    DerivedT &resize(size_t n_newsize) {
        // This is stupid so don't even try :)
        if (n_newsize == m_capacity) {
            return derived_cast();
        }
        ElemT *p_dummy = Malloc::allocate(m_malloc, n_newsize);
        size_t n_oldsize = m_capacity; // need for later deallocation

        // Does the new size extend the buffer or not?
        // true:    Use original `m_index` which is the last written element.
        // false:   Buffer was shortened, so `n_newsize` *is* the last index.
        m_length = (n_newsize > m_capacity) ? m_length : n_newsize;
        m_capacity = n_newsize;

        // Move (not copy!) previous buffer into current cuz slightly faster.
        for (size_t i = 0; i < m_length; i++) {
            p_dummy[i] = std::move(m_buffer[i]);
        }
        // Default construct everything else in the buffer that wasn't moved 
        for (size_t i = m_length; i < m_capacity; i++) {
            Malloc::construct(m_malloc, p_dummy + i);
        }
        // Since previous contents were moved no need to explicitly destroy
        // Also: Malloc::deallocate shouldn't accept nullptr
        if (m_buffer != nullptr) {
            Malloc::deallocate(m_malloc, m_buffer, n_oldsize);
        }
        m_buffer = p_dummy;
        m_iterator.set_range(m_buffer, m_length);
        return derived_cast();
    }

    /**
     * @brief   Gets the last written element and gives it back to you.
     *          It decrements the `m_length` counter and `m_end` iterator.
     * 
     * @attention   This can be shadows, e.g. strings set index's value to 0.
     */
    ElemT pop_back() {
        m_iterator.m_end--;
        return m_buffer[m_length--];
    }

    /**
     * @brief   Deletes/frees buffer, resets the index counter and iterators
     * 
     * @note    `m_capacity` is unaffected. Actually, after freeing the current
     *          buffer, we reallocate fresh memory of the same size!
     */
    DerivedT &clear() {
        destroy_buffer();
        m_buffer = Malloc::allocate(m_malloc, m_capacity);
        for (size_t i = 0; i < m_capacity; i++) {
            Malloc::construct(m_malloc, m_buffer + i);
        }
        m_length = 0;
        m_iterator.set_range(m_buffer);
        return derived_cast();
    }
};
