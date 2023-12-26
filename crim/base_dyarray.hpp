#pragma once

#include <stddef.h>
#include <string.h>

#include <initializer_list>
#include <stdexcept>

// Classes that inherit from parents can be forward declared, but the 
// inheritance itself must only go in the class definition proper.
namespace crim {
    template<class ElemT> struct iterator;

    template<class DerivedT, class ElemT> class base_dyarray;
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
 *              for `m_end` is a bad idea, yet most implementations do this way.
 */
template<class ElemT> struct crim::iterator {
    const ElemT *m_begin; // Address of first written element.
    const ElemT *m_end; // Address right past the last written element.

    iterator(const ElemT *start, const ElemT *stop) 
        : m_begin{start}
        , m_end{stop} 
    {}

    iterator(const iterator &src) 
        : m_begin{src.m_begin}
        , m_end{src.m_end} 
    {}

    static iterator &copy(iterator &lhs, const iterator &rhs) {
        lhs.m_begin = rhs.m_begin;
        lhs.m_end = rhs.m_end;
        return lhs;
    }

    // Copy-assignment.
    iterator &operator=(const iterator &src) {
        return copy(*this, src);
    }

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
 * @tparam  DerivedT   The derived class's name or templated name.
 * @tparam  ElemT       Buffer's element type.
 */
template<class DerivedT, class ElemT> class crim::base_dyarray {
private:
    friend DerivedT; // allows access private and protected members of caller!

    DerivedT &cast_derived() {
        return static_cast<DerivedT &>(*this);
    }

    DerivedT &cast_derived() const {
        return static_cast<DerivedT const&>(*this);
    }

protected:
    static constexpr size_t DYARRAY_STARTCAP = 16;
    static constexpr size_t DYARRAY_MAXLENGTH = 0xFFFFFFFF; // 1-based.
    size_t m_length; // Number of elements written to buffer currently.
    size_t m_capacity; // Number of elements that buffer could hold.
    ElemT *m_buffer; // Heap-allocated 1D array of the specified type.
    iterator<ElemT> m_iterator;

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
        , m_capacity{DYARRAY_STARTCAP}
        , m_buffer{new ElemT[DYARRAY_STARTCAP]}
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
        memcpy(m_buffer, list.begin(), sizeof(ElemT) * m_length);
    }

    /**
     * @brief   Copy-constructor, does deep-copy of all of `source`'s data.
     * 
     * @exception   `std::bad_alloc` due to the call to `new`.
     */
    base_dyarray(const base_dyarray &source) 
        : m_length{source.m_length}
        , m_capacity{source.m_capacity}
        , m_buffer{new ElemT[m_capacity]}
        , m_iterator(source.m_iterator)
    {
        // Prolly faster than strcpy and more generic anyway
        // Only copy up to the last written index, not the total capacity!
        memcpy(m_buffer, source.m_buffer, sizeof(ElemT) * m_length);
    }

    /**
     * @brief   Move-constructor, does a shallow copy of `temp`, which is a
     *          temporary value/expression (usually an rvalue).
     * 
     *          `temp` itself has its internal buffer set to `nullptr` after 
     *          copying so that upon its destruction, the memory isn't freed!
     * 
     * @note    [See here for help.](https://learn.microsoft.com/en-us/cpp/cpp/move-constructors-and-move-assignment-operators-cpp?view=msvc-170)
     */
    base_dyarray(base_dyarray &&temp) 
        : m_length{temp.m_length}
        , m_capacity{temp.m_capacity}
        , m_buffer{temp.m_buffer}
        , m_iterator(temp.m_iterator)
    {
        // erase so that when temp is destroyed we don't free the memory.
        temp.m_buffer = nullptr;
    }

    ~base_dyarray() {
        delete[] m_buffer;
    }

public:
    // ---------------------------- DATA ACCESS --------------------------------

    /**
     * @brief   Gets number of characters written to the internal buffer. 
     *          For strings, this count already excludes nul-termination.
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
    static constexpr size_t max_size() {
        return DYARRAY_MAXLENGTH;
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
        // Copying ourselves results in garbage for the buffer, for some reason.
        if (this != &src) {
            delete[] m_buffer;

            // Copy only up to last written index to avoid unitialized memory.
            m_buffer = new ElemT[src.m_capacity];
            memcpy(m_buffer, src.m_buffer, sizeof(ElemT) * src.m_length);
            m_length = src.m_length;
            m_capacity = src.m_capacity;
            m_iterator = iterator(m_buffer, m_buffer + src.m_length);
        }
        return cast_derived();
    }

    DerivedT &move(base_dyarray &src) {
        // Copying ourselves results in garbage for the buffer, for some reason.
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
        return cast_derived();
    }

    // -------------------------- BUFFER WRITING -------------------------------

    /**
     * @brief   Appends `entry` to the internal buffer. That is, it is added to
     *          the index after the previously written element.
     */
    DerivedT &push_back(ElemT entry) {
        if (m_length > DYARRAY_MAXLENGTH) {
            throw std::length_error("Reached base_dyarray maximum allocations!");
        } else if (m_length + 1 > m_capacity) {
            // All the cool kids seem to grow their buffers by doubling it!
            resize(m_capacity * 2); 
        }
        m_buffer[m_length++] = entry;
        
        // Don't try to dereference unitialized memory and get its address!
        m_iterator.m_end++;
        return cast_derived();
    }

    /**
     * @brief   Reallocates memory for the buffer by extension or narrowing.
     * 
     * @details Similar to C's `realloc`. Cleans up the old memory as well.
     *          Updates internals for you accordingly so you don't have to!
     * 
     * @param   new_size    Number of elements to increase or decrease by.
     * 
     * @warning For strings, this will not guarantee nul termination at all.
     */
    DerivedT &resize(size_t n_newsize) {
        // This is stupid so don't even try :)
        if (n_newsize == m_capacity) {
            return cast_derived();
        }
        // Dummy since memcpy isn't guaranteed to work for overlapping blocks.
        ElemT *p_dummy = new ElemT[n_newsize];

        // Does the new size extend the buffer or not?
        bool b_extended = (n_newsize > m_capacity);

        // true:    Copy only up to last element written to.
        // false:   Copy up to requested size as we'll shorten the buffer.
        size_t n_copysize = (b_extended) ? m_length : n_newsize;

        // Remember C-style memory manipulation requires size in bytes/chars!
        memcpy(p_dummy, m_buffer, sizeof(ElemT) * n_copysize);
        delete[] m_buffer;
        m_buffer = p_dummy;

        // true:    Maintain the index of the last written element.
        // false:   Buffer was shortened, so new size *is* the last index.
        m_length = (b_extended) ? m_length : n_newsize;
        m_capacity = n_newsize;
        m_iterator.m_begin = p_dummy;
        m_iterator.m_end = p_dummy + n_copysize; // past end of new memblock
        return cast_derived();
    }

    /**
     * @brief   Gets the last written element and gives it back to you.
     *          At the same time, it decrements the internal index counter.
     * 
     * @attention   This can be overriden, e.g. strings set index's value to 0.
     */
    virtual ElemT pop_back() {
        return m_buffer[m_length--];
    }

    /**
     * @brief   Deletes/frees buffer, resets the index counter and iterators
     * 
     * @attention   May be overriden if a specialization has different needs.
     * 
     * @note    `m_capacity` is unaffected. Actually, after freeing the current
     *          buffer, we reallocate fresh memory of the same size!
     */
    virtual DerivedT &clear() {
        // Tempting to do away with this, but would leak memory if buffer elems
        // were also dynamically allocated!
        delete[] m_buffer;
        m_buffer = new ElemT[m_capacity];
        m_length = 0;
        m_iterator = iterator(m_buffer, m_buffer);
        return cast_derived();
    }
};
