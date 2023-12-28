#pragma once

#include <stddef.h>
#include <string.h>

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <functional>

// Classes that inherit from parents can be forward declared, but the 
// inheritance itself must only go in the class definition proper.
namespace crim {
    template<class ElemT> struct iterator;

    template<class DerivedT, typename ElemT> class base_dyarray;
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
    using ptr = const ElemT*;
    ptr m_begin; // Address of first written element.
    ptr m_end; // Address right past the last written element.

    iterator(ptr start, ptr stop) : m_begin{start}, m_end{stop} {}

    iterator(const iterator &src) : iterator(src.m_begin, src.m_end) {}

    iterator(const iterator &&src) : iterator(src.m_begin, src.m_end) {}

    void set_pointers(ptr start, ptr stop) {
        m_begin = start;
        m_end = stop;
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
template<class DerivedT, typename ElemT> class crim::base_dyarray {
private:
    friend DerivedT; // allows access private and protected members of caller!

    DerivedT &derived_cast() {
        return static_cast<DerivedT &>(*this);
    }

    DerivedT &derived_cast() const {
        return static_cast<DerivedT const&>(*this);
    }

protected:
    static constexpr size_t START_CAPACITY = 16;
    static constexpr size_t MAX_LENGTH = 0xFFFFFFFF; // 1-based.
    size_t m_length = 0; // Number of elements written to buffer currently.
    size_t m_capacity = 0; // Number of elements that buffer could hold.
    ElemT *m_buffer = nullptr; // Heap-allocated 1D array of the specified type.
    iterator<ElemT> m_iterator;
    // std::allocator<ElemT> m_alloc;

    // -------------------- CONSTRUCTORS/DESTRUCTORS ---------------------------
    // NOTICE: These are protected to avoid users calling these by themselves!
    // See: https://www.reddit.com/r/cpp/comments/lhvkzs/comment/gn3nmsx/

    /**
     * @brief   Basic constructor, inheritors usually call this automatically.
     *          Creates an empty buffer with some initial allocations.
     * 
     * @exception   `std::bad_alloc` due to the call to `new`.
     */
    base_dyarray() 
        : m_length{0}
        , m_capacity{0}
        , m_buffer{nullptr}
        , m_iterator(m_buffer, m_buffer)
    {}

    /**
     * @brief   Constructor for "array literals" (curly brace literals) which
     *          are really implemented via `std::initializer_list` objects.
     * 
     *          This copies all the elements of the list into our buffer.
     * 
     *          Upon an append/push_back, this will likely cause a realloc.
     * 
     * @exception   `std::bad_alloc` due to the call to `new`.
     */
    base_dyarray(std::initializer_list<ElemT> list) 
        : m_length{list.size()}
        , m_capacity{m_length}
        , m_buffer{new ElemT[m_length]}
        , m_iterator(m_buffer, m_buffer + m_length)
    {
        std::copy(list.begin(), list.end(), m_buffer);
    }

    /**
     * @brief   Copy-constructor, does deep-copy of all of `src`'s data.
     * 
     * @exception   `std::bad_alloc` due to the call to `new`.
     */
    base_dyarray(const base_dyarray &src) 
        : m_length{src.m_length}
        , m_capacity{src.m_capacity}
        , m_buffer{new ElemT[m_capacity]}
        , m_iterator(m_buffer, m_buffer + m_length)
    {
        std::copy(src.begin(), src.end(), m_buffer);
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
        : m_length{tmp.m_length}
        , m_capacity{tmp.m_capacity}
        , m_buffer{tmp.m_buffer}
        , m_iterator(tmp.m_iterator)
    {
        // erase so that when tmp is destroyed we don't free the memory.
        tmp.m_buffer = nullptr;
    }

    ~base_dyarray() {
        delete[] m_buffer;
    }

public:
    // ---------------------------- DATA ACCESS --------------------------------

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
     * @brief   See if begin and end iterators point to the exact same address. 
     * 
     *          This is because on empty initialization, they do! But once at 
     *          least one element is pushed back, the end pointer increments.
    */
    bool empty() const {
        return m_iterator.m_begin == m_iterator.m_end;
    }

    /**
     * @brief   Iterator start. Always points to the very first element, which
     *          is located at the internal buffer's index 0. 
     * 
     *          This is meant to be used in the compiler-generated instructions 
     *          that implement a range-based for loop.
     * 
     * @note    This (should be) safe to dereference, unlike `.end()`.
     */
    const ElemT *begin() const {
        return m_iterator.m_begin;
    }

    /**
     * @brief   Iterator control/terminator. Always points to 1 past the most 
     *          recently index. 
     * 
     *          This is meant to be used in the compiler-generated instructions 
     *          that implement a range-based for loop.
     * 
     * @warning *DO NOT* attempt to dereference this! It is out of bounds!
     */
    const ElemT *end() const {
        return m_iterator.m_end;
    }

    /**
     * @brief   Maximum allowable elements the buffer can be allocated for.
     * 
     * @note    static constexpr member functions cannot be const functions.
     */
    static constexpr size_t max_length() {
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

    // ---------------------- DATA COPYING/MOVEMENT ----------------------------
    // Assignment operator overloads cannot be called from the child directly. 
    // You'll need a wrapper like `crim::string &operator=(crim::string &&src)`.

    DerivedT &copy(const base_dyarray &src) {
        // Don't copy ourselves because memcpy on overlaps won't end well.
        if (this != &src) {
            delete[] m_buffer;

            // Copy only up to last written index to avoid unitialized memory.
            m_buffer = new ElemT[src.m_capacity];

            // Don't call memcpy as it won't work for non-trivial types!
            // std::copy calls copy-constructor for each non-trivial entity.
            std::copy(src.begin(), src.end(), m_buffer);
            m_length = src.m_length;
            m_capacity = src.m_capacity;
            m_iterator.set_pointers(m_buffer, m_buffer + m_length);
        }
        return derived_cast();
    }

    DerivedT &move(base_dyarray &&src) {
        // If we try to move ourselves, we'll free the same buffer!
        if (this != &src) {
            delete[] m_buffer; 

            // Do a shallow copy, assuming `src.m_buffer` is also allocated.
            m_buffer = src.m_buffer;
            m_length = src.m_length;
            m_capacity = src.m_capacity;
            m_iterator = src.m_iterator;

            // Make null so our memory won't be freed just yet.
            src.m_buffer = nullptr;
        }
        return derived_cast();
    }

    // -------------------------- BUFFER WRITING -------------------------------

private:
    using push_callback = std::function<void(ElemT &target)>;

    /**
     * @brief   For strings, Valgrind tells me that using the `x > y` comparison 
     *          results in invalid reads/writes of 1. Using `x >= y` fixes it.
     * 
     * @param   assignment_fn   A lambda that assigns to `m_buffer[m_length]`.
     *                          This is meant to help generalize between
     *                          `const ElemT &` and `ElemT &&`.
     * 
     * @note    I've opted instead to make `crim::dystring::append` decrement
     *          `m_length` right after calling `base::push_back('\0')`.
     *          This should hopefully this easier to work with for non-strings.
     */
    DerivedT &push_helper(push_callback assignment_fn) {
        if (m_length > MAX_LENGTH) {
            // Also helpful to indicate when I accidentally overflow
            throw std::length_error("Reached crim::base_dyarray::MAXLENGTH!");
        } else if (m_length + 1 > m_capacity) {
            // All the cool kids seem to grow their buffers by doubling it!
            resize((m_capacity == 0) ? 1 : m_capacity * 2); 
        }

        assignment_fn(m_buffer[m_length++]);

        // Don't try to dereference unitialized memory and get its address!
        m_iterator.m_end++;
        return derived_cast();
    }

public:
    /**
     * @brief   Moves `entry` to the top of the internal buffer. That is, it is 
     *          moved to the index after the previously written element. 
     * 
     *          It's mainly helpful for objects with a clear move-assigment,
     *          such as by overloading the `=` operator. For example, see:
     *          `crim::dystring &operator=(crim::dystring &&)`.
     * 
     * @note    We take an rvalue reference so `entry` itself maybe invalidated!
     *          This function only really works with temporary values. 
     */
    DerivedT &push_back(ElemT &&entry) {
        // Captures `&&entry` and call move-assignment specifically to avoid
        // garbage as rvalues are destroyed very easily.
        return push_helper([&](ElemT &target) {
            target = std::move(entry);
        });
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
        return push_helper([&](ElemT &target) {
            target = entry;
        });
    }

    /**
     * @brief   Reallocates memory for the buffer by extension or narrowing.
     * 
     * @details Similar to C's `realloc`. Cleans up the old memory as well.
     *          Updates internals for you accordingly so you don't have to!
     * 
     * @param   new_size    New requested buffer size, may be greater or lesser.
     * 
     * @warning For strings, this may not guarantee nul termination. Although we
     *          do 0-initialize the memory.
     */
    DerivedT &resize(size_t n_newsize) {
        // This is stupid so don't even try :)
        if (n_newsize == m_capacity) {
            return derived_cast();
        }
        // Add 1 to copy correct number of elements w/o touching bad memory.
        ElemT *p_dummy = new ElemT[n_newsize];

        // Does the new size extend the buffer or not?
        // true:    Use original `m_index` which is the last written element.
        // false:   Buffer was shortened, so `n_newsize` *is* the last index.
        m_length = (n_newsize > m_capacity) ? m_length : n_newsize;
        m_capacity = n_newsize;
        m_iterator.m_begin = p_dummy;
        m_iterator.m_end = p_dummy + m_length; // past end of new memblock

        // We can't rely on memcpy, as when our buffer is of containers,
        // we'll not call any copy or move assignments and cause double frees!
        for (size_t i = 0; i < m_length; i++) {
            // Try to call this object's move assignment.
            p_dummy[i] = std::move(m_buffer[i]);
        }
        delete[] m_buffer;
        m_buffer = p_dummy;
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
        // Tempting to do away with this, but would leak memory if buffer elems
        // were also dynamically allocated!
        delete[] m_buffer;
        m_buffer = new ElemT[m_capacity];
        m_length = 0;
        m_iterator.set_pointers(m_buffer, m_buffer);
        return derived_cast();
    }
};
