#pragma once

#include <stdio.h> 
#include <stddef.h> // size_t
#include <string.h> // memcpy
#include <stdexcept>

// Classes that inherit from parents can be forward declared, but the 
// inheritance itself must only go in the class definition proper.
namespace crim {
    template<class ElemT> struct Iterator;

    template<class ClassT, class ElemT> class base_dyarray;

    template<class ElemT> class dyarray;
};

/**
 * @brief   An iterator class for use with range-based for loops.
 * 
 * @details You'll have to provide `begin()` and `end()` class methods yourself.
 * 
 *          Such functions must return pointers to the template type. For help,
 *          see my dyarray base class implementation for an example.
 * 
 * @tparam  ElemT 
 * 
 * @note    Valgrind suggests that using the an out of bounds memory address
 *          for `m_end` is a bad idea, so I opted to make it in bounds.
 */
template<class ElemT> struct crim::Iterator {
    ElemT *m_begin; // Address of first written element.
    ElemT *m_end; // Address of last written element.

    // Start both pointers at the same address to indicate an empty buffer.
    Iterator(ElemT *source) 
        : m_begin{source}
        , m_end{source}
    {}

    // Copy-constructor
    Iterator(Iterator &source) 
        : m_begin{source.m_begin}
        , m_end{source.m_end}
    {}
};

/**
 * @brief   A somewhat generic growable array that can be templated over.
 * 
 *          For templates, you need to pass the templated name,
 *          e.g: `ClassT<ElemT>`.
 * 
 * @details This utilizes the Continuous Recurring Template Pattern (CRTP).
 *          Imagine it like a template class for template classes.
 * 
 *          With this, descendent templates inherit & specialize from this one.
 * 
 *          e.g: template<ElemT> 
 *          class crim::string : crim::base_dyarray<crim::string<ElemT>, ElemT>
 *          {...}
 * 
 *          Although verbose, this allows us to reuse implementation!
 * 
 *          For more information, see: https://stackoverflow.com/a/56423659
 * 
 * @tparam  ClassT The derived class's name or templated name.
 * @tparam  ElemT Buffer's element type.
 */
template<class ClassT, class ElemT> class crim::base_dyarray {
private:
    friend ClassT; // allows access private and protected members of caller!

    // Allows access to derived types.
    ClassT &cthis() {
        return static_cast<ClassT &>(*this);
    }

    // Overload for const references to derived types.
    ClassT &cthis() const {
        return static_cast<ClassT const&>(*this);
    }

protected:
    static constexpr size_t DYARRAY_STARTCAP = 16;
    static constexpr size_t DYARRAY_MAXLENGTH = 0xFFFFFFFF; // 1-based.
    size_t m_index; // 0-based number of elements written to buffer currently.
    size_t m_capacity; // 1-based number of elements that buffer could hold.
    ElemT *m_buffer; // 1D buffer of the specified type.
    Iterator<ElemT> m_iterator;

public:
    // -------------------- CONSTRUCTORS/DESTRUCTORS ---------------------------

    // Primary constructor, does some basic initial allocations. May throw!
    base_dyarray() 
        : m_index{0}
        , m_capacity{DYARRAY_STARTCAP}
        , m_buffer{new ElemT[DYARRAY_STARTCAP]}
        , m_iterator(m_buffer)
    {}

    // Copy-constructor, does deep-copy of internal buffer. May throw!
    base_dyarray(const base_dyarray &source) 
        : m_index{source.m_index}
        , m_capacity{source.m_capacity}
        , m_buffer{new ElemT[m_capacity]}
        , m_iterator(source.m_iterator)
    {
        // Prolly faster than strcpy and more generic anyway
        // Only copy up to the last written index, not the total capacity!
        memcpy(m_buffer, source.m_buffer, sizeof(ElemT) * m_index);
    }

    ~base_dyarray() {
        delete[] m_buffer;
    }

    // ---------------------------- DATA ACCESS --------------------------------

    // Number of elements currently written to the buffer.
    // For strings, this likely does not include the nul terminator.
    size_t size() const {
        return m_index;
    }

    // Number of elements our buffer can hold at most currently.
    size_t capacity() const {
        return m_capacity;
    }

    // Tests if the beginning and end iterators point to the exact same address.
    bool empty() const {
        return begin() == end();
    }

    ElemT *begin() const {
        return m_iterator.m_begin;
    }

    ElemT *end() const {
        return m_iterator.m_end;
    }

    // Maximum allowable number of elements the buffer can be allocated for.
    static constexpr size_t max_size() {
        return DYARRAY_MAXLENGTH;
    }

    /**
     * @brief   Read-only pointer to the internal buffer. Useful for strings.
     * 
     * @note    See [cppreference.com.](https://en.cppreference.com/w/cpp/container/vector/data)
     */
    const ElemT *data() const {
        return m_buffer;
    }

    ElemT &at(size_t index) {
        if (index > m_index) {
            throw std::out_of_range("Requested base_dyarray index is invalid!");
        }
        return m_buffer[index];
    }

    ElemT &operator[](size_t index) {
        return at(index);
    }

    // -------------------------- BUFFER WRITING -------------------------------

    ClassT &push_back(ElemT entry) {
        // Check for this first cuz who knows what can happen!
        if (m_index > DYARRAY_MAXLENGTH) {
            throw std::length_error("Reached base_dyarray maximum allocations!");
        } else if (m_index + 1 > m_capacity) {
            // All the cool kids seem to grow their buffers by doubling it!
            resize(m_capacity * 2); 
        }
        m_buffer[m_index++] = entry;
        m_iterator.m_end = &m_buffer[m_index];
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
    ClassT &resize(size_t new_size) {
        // This is stupid so don't even try :)
        if (new_size == m_capacity) {
            return cthis();
        }
        // Dummy since memcpy isn't guaranteed to work for overlapping blocks.
        ElemT *dummy = new ElemT[new_size];

        bool extended = (new_size > m_capacity);

        // true:    Copy only up to last element written to.
        // false:   Copy up to requested size as we'll shorten the buffer.
        size_t copysize = (extended) ? m_index : new_size;

        // Remember C-style memory manipulation requires size in bytes/chars!
        memcpy(dummy, m_buffer, sizeof(ElemT) * copysize);
        delete[] m_buffer;
        m_buffer = dummy;

        // true:    Maintain the index of the last written element.
        // false:   Buffer was shortened, so new size is the last index.
        m_index = (extended) ? m_index : new_size;
        m_capacity = new_size;
        m_iterator.m_begin = dummy;
        m_iterator.m_end = dummy + copysize; // past end of new memory block
        return cthis();
    }

    /**
     * @brief Gets last written element and decrements internal index counter.
     * 
     * @note This can be overriden, e.g. strings set index's value to 0.
     */
    virtual ElemT pop_back() {
        return m_buffer[m_index--];
    }

    /**
     * @brief Deletes your current buffer and resets the index counter.
     * 
     * @note `m_capacity` is unaffected and we actually reallocate fresh memory.
     * 
     * @note This can be overriden if a specialization has different needs.
     */
    virtual ClassT &clear() {
        delete[] m_buffer;
        m_buffer = new ElemT[m_capacity];
        m_index = 0;
        return cthis();
    }
};

/**
 * @brief   Programmer-facing version of `crim::base_dyarray`. You're intended 
 *          to use this with fundamental/user-defined types only.
 * 
 * @tparam  ElemT Desired type of the buffer's elements.
 * 
 * @warning This is a work in progress!
 */
template<class ElemT> 
class crim::dyarray : public crim::base_dyarray<crim::dyarray<ElemT>, ElemT> {
private:
    using base_class = base_dyarray<dyarray<ElemT>, ElemT>;
public:
    // TODO: Figure out how to initiliaze array literals to buffer
    // Prolly look at smth like std::initializer_list
};
