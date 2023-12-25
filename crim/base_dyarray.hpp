#pragma once

#include <stdio.h> 
#include <stddef.h>
#include <string.h>

#include <initializer_list>
#include <stdexcept>

// Classes that inherit from parents can be forward declared, but the 
// inheritance itself must only go in the class definition proper.
namespace crim {
    template<class ElemT> struct iterator;

    template<class ConcreteT, class ElemT> class base_dyarray;
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

    // Start both pointers at the same address to indicate an empty buffer.
    iterator(const ElemT *source) 
        : m_begin{source}
        , m_end{source}
    {}

    // Useful with std::initializer_list instances.
    iterator(const ElemT *start, const ElemT *stop)
        : m_begin{start}
        , m_end{stop}
    {}

    // Copy-constructor.
    iterator(const iterator &source) 
        : m_begin{source.m_begin}
        , m_end{source.m_end}
    {}
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
 *          `template<ChT>class string : base_dyarray<string<ChT>, ChT>`
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
 * @tparam  ConcreteT   The derived class's name or templated name.
 * @tparam  ElemT       Buffer's element type.
 */
template<class ConcreteT, class ElemT> class crim::base_dyarray {
private:
    friend ConcreteT; // allows access private and protected members of caller!

    // Allows access to derived types.
    ConcreteT &cthis() {
        return static_cast<ConcreteT &>(*this);
    }

    // Overload for const references to derived types.
    ConcreteT &cthis() const {
        return static_cast<ConcreteT const&>(*this);
    }

protected:
    static constexpr size_t DYARRAY_STARTCAP = 16;
    static constexpr size_t DYARRAY_MAXLENGTH = 0xFFFFFFFF; // 1-based.
    size_t m_size; // Number of elements written to buffer currently.
    size_t m_capacity; // Number of elements that buffer could hold.
    ElemT *m_buffer; // Heap-allocated 1D array of the specified type.
    iterator<ElemT> m_iterator;

public:
    // -------------------- CONSTRUCTORS/DESTRUCTORS ---------------------------

    /**
     * @brief   Basic constructor, inheritors usually call this automatically.
     *          Creates an empty buffer with some initial allocations.
     * 
     * @exception   `std::bad_alloc` due to the call to `new`.
     */
    base_dyarray() 
        : m_size{0}
        , m_capacity{DYARRAY_STARTCAP}
        , m_buffer{new ElemT[DYARRAY_STARTCAP]}
        , m_iterator(m_buffer)
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
        : m_size{list.size()}
        , m_capacity{m_size}
        , m_buffer{new ElemT[m_size]}
        , m_iterator(m_buffer, m_buffer + m_size)
    {
        memcpy(m_buffer, list.begin(), sizeof(ElemT) * m_size);
    }

    /**
     * @brief   Copy-constructor, does deep-copy of all of `source`'s data.
     * 
     * @exception   `std::bad_alloc` due to the call to `new`.
     */
    base_dyarray(const base_dyarray &source) 
        : m_size{source.m_size}
        , m_capacity{source.m_capacity}
        , m_buffer{new ElemT[m_capacity]}
        , m_iterator(source.m_iterator)
    {
        // Prolly faster than strcpy and more generic anyway
        // Only copy up to the last written index, not the total capacity!
        memcpy(m_buffer, source.m_buffer, sizeof(ElemT) * m_size);
    }

    ~base_dyarray() {
        delete[] m_buffer;
    }

    // ---------------------------- DATA ACCESS --------------------------------

    /**
     * @brief   Returns number of characters written to the internal buffer. 
     *          For strings, this count already excludes nul-termination.
     */
    size_t length() const {
        return this->m_size;
    }

    // Number of elements our buffer can hold at most currently.
    size_t capacity() const {
        return m_capacity;
    }

    // Tests if the beginning and end iterators point to the exact same address.
    bool empty() const {
        return m_iterator.m_begin == m_iterator.m_end;
    }

    // Iterator start, points to the very first element (index 0).
    constexpr const ElemT *begin() const noexcept {
        return m_iterator.m_begin;
    }

    // Iterator control, points to 1 past the most recently written element.
    constexpr const ElemT *end() const noexcept {
        return m_iterator.m_end;
    }

    // Maximum allowable number of elements the buffer can be allocated for.
    static constexpr size_t max_size() {
        return DYARRAY_MAXLENGTH;
    }

    /**
     * @brief   Read-only pointer to the internal buffer. Useful for strings.
     * 
     * @warning If you attempt to modify the buffer from here, good luck!
     * 
     * @note    See [cppreference.com.](https://en.cppreference.com/w/cpp/container/vector/data)
     */
    const ElemT *data() const {
        return m_buffer;
    }

    ElemT &at(size_t index) {
        if (index > m_size) {
            throw std::out_of_range("Requested base_dyarray index is invalid!");
        }
        return m_buffer[index];
    }

    ElemT &operator[](size_t index) {
        return at(index);
    }

    // -------------------------- BUFFER WRITING -------------------------------

    /**
     * @brief   Appends `entry` to the internal buffer. That is, it is added to
     *          the index after the previously written element.
     */
    ConcreteT &push_back(ElemT entry) {
        // Check for this first cuz who knows what can happen!
        if (m_size > DYARRAY_MAXLENGTH) {
            throw std::length_error("Reached base_dyarray maximum allocations!");
        } else if (m_size + 1 > m_capacity) {
            // All the cool kids seem to grow their buffers by doubling it!
            resize(m_capacity * 2); 
        }
        m_buffer[m_size++] = entry;
        // Since m_size already incremented, this points to 1 past the lastest
        m_iterator.m_end = &m_buffer[m_size];
        return cthis();
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
    ConcreteT &resize(size_t new_size) {
        // This is stupid so don't even try :)
        if (new_size == m_capacity) {
            return cthis();
        }
        // Dummy since memcpy isn't guaranteed to work for overlapping blocks.
        ElemT *dummy = new ElemT[new_size];

        bool extended = (new_size > m_capacity);

        // true:    Copy only up to last element written to.
        // false:   Copy up to requested size as we'll shorten the buffer.
        size_t n_copysize = (extended) ? m_size : new_size;

        // Remember C-style memory manipulation requires size in bytes/chars!
        memcpy(dummy, m_buffer, sizeof(ElemT) * n_copysize);
        delete[] m_buffer;
        m_buffer = dummy;

        // true:    Maintain the index of the last written element.
        // false:   Buffer was shortened, so new size is the last index.
        m_size = (extended) ? m_size : new_size;
        m_capacity = new_size;
        m_iterator.m_begin = dummy;
        m_iterator.m_end = dummy + n_copysize + 1; // past end of new memblock
        return cthis();
    }

    /**
     * @brief   Gets the last written element and gives it back to you.
     *          At the same time, it decrements the internal index counter.
     * 
     * @attention   This can be overriden, e.g. strings set index's value to 0.
     */
    virtual ElemT pop_back() {
        return m_buffer[m_size--];
    }

    /**
     * @brief   Deletes/frees current buffer, resets the index counter.
     * 
     * @attention   May be overriden if a specialization has different needs.
     * 
     * @note    `m_capacity` is unaffected. Actually, after freeing the current
     *          buffer, we reallocate fresh memory of the same size!
     */
    virtual ConcreteT &clear() {
        delete[] m_buffer;
        m_buffer = new ElemT[m_capacity];
        m_size = 0;
        return cthis();
    }
};
