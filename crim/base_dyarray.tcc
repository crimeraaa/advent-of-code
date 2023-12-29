#pragma once
/*- -*- ---***--- -*-        THE C STANDARD LIBRARY       -*- ---***---  -*- -*/
#include <stddef.h>
#include <string.h>
/*- -*- ---***--- -*-       THE C++ STANDARD LIBRARY      -*- ---***---  -*- -*/
#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <functional>
/*- -*- ---***--- -*-    REINVENTING THE WHEEL LIBRARY    -*- ---***---  -*- -*/
#include "iterator.tcc"
/*- -*- ---***--- -*-                                     -*- ---***---  -*- -*/

/**
 * @brief   Classes that inherit from parents can be forward declared, but the
 *          inheritance itself must only go in the class definition proper. 
 * 
 *          However, default values must go in the forward declarations!
 */
namespace crim {
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
     * @tparam  AllocT      Desired allocator, based off of `std::allocator`.
     */
    template<
        class DerivedT, class ElemT, class AllocT = std::allocator<ElemT>
    > class base_dyarray;
};

template<class DerivedT, class ElemT, class AllocT> class crim::base_dyarray {
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
    static constexpr size_t DYARRAY_START_CAPACITY = 16;
    static constexpr size_t DYARRAY_MAX_CAPACITY = 0xFFFFFFFF; // 1-based.
    AllocT m_malloc;
    size_t m_nlength; // Number of elements written to buffer currently.
    size_t m_ncapacity; // Number of elements that buffer could hold.
    ElemT *m_pbuffer; // Heap-allocated 1D array of the specified type.
    iterator<ElemT> m_iterator;

    /* -*- ---***--- -*-      CONSTRUCTORS, DESTRUCTORS     -*- ---***--- -*- */
    // NOTICE: These are protected to avoid users calling these by themselves!
    // See: https://www.reddit.com/r/cpp/comments/lhvkzs/comment/gn3nmsx/

    // Primary delegated constructor.
    base_dyarray(size_t n_length, size_t n_capacity, ElemT *p_mem)
    : m_malloc{}
    , m_nlength{n_length}
    , m_ncapacity{n_capacity}
    , m_pbuffer{p_mem}
    , m_iterator(m_pbuffer, m_nlength) 
    {}

    // Default constructor, zeroes out the memory.
    base_dyarray() : base_dyarray(0, 0, nullptr) {}
    
    /**
     * @brief   Secondary delegated constructor which takes care of allocating 
     *          the correct amount of memory. It in turn delegates to the
     *          primary delegated constructor.
     */
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
    : base_dyarray(list.size(), list.size()) {
        // this->begin() should be the non-const version even if we're const
        std::copy(list.begin(), list.end(), begin());
    }

    /**
     * @brief   Copy-constructor, it allocates enough memory for the buffer
     *          hold `src`'s data buffer, then it deep-copies them.
     */
    base_dyarray(const base_dyarray &src) 
    : base_dyarray(src.length(), src.capacity()) {
        // this->begin() should be the non-const version even if we're const
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
    : base_dyarray(tmp.length(), tmp.capacity(), tmp.begin()) {
        tmp.reset();
    }

    /**
     * @brief   Destroys all constructed objects in the buffer for the range 
     *          `m_pbuffer[0 ... length()-1]`. 
     * 
     *          We do this before deallocating the pointer so we can call 
     *          destructors for non-trivially destructible objects.
     * 
     * @note    Anything past `length()` should be uninitialized so there's no
     *          need to manually destroy `m_pbuffer[length()-1 ... capacity()]`.
     */
    ~base_dyarray() {
        // Malloc::deallocate doesn't accept nullptr so let's avoid that.
        if (m_pbuffer == nullptr) {
            return;
        }
        // Destroy all constructed objects we have, because memory is hard.
        for (size_t i = 0; i < length(); i++) {
            Malloc::destroy(m_malloc, &m_pbuffer[i]);
        }
        // After instances are destroyed then we can get rid of the pointer.
        Malloc::deallocate(m_malloc, m_pbuffer, capacity());
    }

private:
    /**
     * @brief   Effectively zeroes out the memory.
     * 
     *          Primarily to erase `m_pbuffer` of temporary instances so that
     *          when they're destroyed the memory pointed is safe from deletion.
     * 
     * @warning Assumes you've taken care of freeing/moving memory properly!
     */
    void reset() {
        m_nlength = 0;
        m_ncapacity = 0;
        m_pbuffer = nullptr;
        m_iterator.set_range(nullptr, 0);
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
        if (index > m_nlength) {
            throw std::out_of_range("Requested base_dyarray index is invalid!");
        }
        return m_pbuffer[index];
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
     *          This is mainly meant for strings, where the buffer is usually
     *          (or supposed to be) nul-terminated.
     * 
     * @warning Be careful with updating the buffer this way! If you get into
     *          trouble, all I can say is good luck!
     */
    ElemT *data() {
        return m_pbuffer;
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
        return m_nlength;
    }

    /** 
     * @brief   Gets number of elements our buffer can hold at most currently. 
     *          In other words, this is how much space is allocated for the 
     *          buffer right now.
    */
    size_t capacity() const {
        return m_ncapacity;
    }

    /**
     * @brief   Maximum allowable elements the buffer can be allocated for.
     * 
     * @note    static constexpr member functions cannot be const functions.
     */
    static constexpr size_t max_length() {
        return DYARRAY_MAX_CAPACITY;
    }

    /**
     * @brief   Read-only pointer to the internal buffer. Useful for strings.
     *          For string classes, this should return nul terminated data. 
     * 
     * @note    See [cppreference.com.](https://en.cppreference.com/w/cpp/container/vector/data)
     */
    const ElemT *data() const {
        return m_pbuffer;
    }

    /* -*- ---***--- -*-              ITERATORS             -*- ---***--- -*- */

    /**
     * @brief   Get a read-write iterator that points to the very first element
     *          in the buffer.
     */
    ElemT *begin() {
        return m_iterator.begin();
    }

    /**
     * @brief   Get a read-write iterator that points to an address 1 past the
     *          last written element.
     *
     * @warning Don't dereference this if you know what's good for you!
     */
    ElemT *end() {
        return m_iterator.end();
    }
    
    /**
     * @brief   Get a read-only iterator that points to the very first element
     *          in the buffer.
     */
    const ElemT *begin() const {
        return m_iterator.begin();
    }

    /**
     * @brief   Get a read-only iterator that points to an address 1 past the
     *          last written element.
     *
     * @warning Don't dereference this if you know what's good for you!
     */
    const ElemT *end() const {
        return m_iterator.end();
    }

    /* -*- ---***--- -*-      DATA COPYING AND MOVEMENT     -*- ---***--- -*- */
    // Assignment operator overloads cannot be called from the child directly. 
    // You'll need a wrapper like `crim::string &operator=(crim::string &&src)`.

    DerivedT &copy(const base_dyarray &src) {
        // Don't copy ourselves, copying overlapping memory won't end well.
        if (this != &src) {
            // Clear any constructed instances and heap-allocated memory.
            this->~base_dyarray();

            // Deep-copy only up to last written index to avoid unitialized 
            // memory. std::copy calls copy-constructor for each non-trivial.
            m_pbuffer = Malloc::allocate(m_malloc, src.capacity());
            std::copy(src.begin(), src.end(), begin());

            m_nlength = src.m_nlength;
            m_ncapacity = src.m_ncapacity;
            m_iterator.set_range(m_pbuffer, m_nlength);
        }
        return derived_cast();
    }

    DerivedT &move(base_dyarray &&src) {
        // If we try to move ourselves, we'll destroy the same buffer!
        if (this != &src) {
            // Clear any constructed instances and heap-allocated memory.
            this->~base_dyarray();

            // Do a shallow copy, since `src` will be destroyed shortly anyway.
            m_pbuffer = src.begin();
            m_nlength = src.length();
            m_ncapacity = src.capacity();
            m_iterator = src.m_iterator;

            // `src.m_pbuffer` is set to null so it's safe from deletion now.
            src.reset();
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
        // `entry`, being named, "decays" to an lvalue reference so do this
        // so we can call the move constructor.
        return push_back_helper([&]() {
            m_pbuffer[m_nlength++] = std::move(entry);
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
        return push_back_helper([&]() {
            m_pbuffer[m_nlength++] = entry;
        });
    }

private:
    /**
     * @brief   Simple wrapper to help us generalize for either: 
     *          `const ElemT &` (constant lvalue reference, named instance) 
     *          `ElemT &&` (writeable rvalue reference, temporary instance)
     * 
     * @param   assign_fn   A lambda function or a callback function. Capture
     *                      everything by reference (and `this` by value) then
     *                      do whatever you need in the function body.
     */
    DerivedT &push_back_helper(std::function<void(void)> assign_fn) {
        if (m_nlength > DYARRAY_MAX_CAPACITY) {
            throw std::length_error("Reached crim::base_dyarray::MAXLENGTH!");
        } else if (m_nlength + 1 > m_ncapacity) {
            // All the cool kids seem to grow their buffers by doubling it!
            resize(empty() ? DYARRAY_START_CAPACITY : m_ncapacity * 2); 
        }
        // Important if non-trivial constructors/heap-allocations are involved!
        Malloc::construct(m_malloc, &m_pbuffer[m_nlength]);

        // Lets us generalize for const lvalue reference or rvalue reference.
        assign_fn();

        // Don't try to dereference unitialized memory and get its address!
        m_iterator.m_end++;
        return derived_cast();
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
        if (n_newsize == m_ncapacity) {
            return derived_cast();
        }
        ElemT *p_dummy = Malloc::allocate(m_malloc, n_newsize);

        // Need for later so we deallocate the correct number of elements.
        size_t n_oldsize = m_ncapacity; 

        // Does the new size extend the buffer or not?
        // true:    Use current `m_index` since any farther is uninitialized.
        // false:   Buffer was shortened, so `n_newsize` *is* the last index.
        m_nlength = (n_newsize > m_ncapacity) ? m_nlength : n_newsize;
        m_ncapacity = n_newsize;

        // Move, not copy, previous buffer into current since it's abit faster.
        for (size_t i = 0; i < m_nlength; i++) {
            p_dummy[i] = std::move(m_pbuffer[i]);
        }

        // Since previous contents were moved no need to explicitly destroy
        // Also: Malloc::deallocate shouldn't accept `nullptr`!
        if (m_pbuffer != nullptr) {
            Malloc::deallocate(m_malloc, m_pbuffer, n_oldsize);
        }
        m_pbuffer = p_dummy;
        m_iterator.set_range(m_pbuffer, m_nlength);
        return derived_cast();
    }

    /**
     * @brief   Copies the last written element and gives it back to you.
     * 
     *          It decrements the `m_nlength` counter and `m_end` iterator.
     * 
     *          The index at which the element was is destroyed just to be safe.
     * 
     * @attention   This can be shadowed, e.g. strings set index's value to 0.
     */
    ElemT pop_back() {
        ElemT top = m_pbuffer[m_nlength - 1]; // copy by value
        // Destructor only takes care of up to m_nlength elements, so need this!
        Malloc::destroy(m_malloc, m_pbuffer + m_nlength - 1);
        m_iterator.m_end--;
        m_nlength--;
        return top;
    }

    /**
     * @brief   Deletes/frees buffer, resets the index counter and iterators.
     * 
     * @note    `m_ncapacity` is unaffected. Actually, after freeing the current
     *          buffer we reallocate fresh, unitialized memory of the same size!
     */
    DerivedT &clear() {
        // Destroy all created objects and free our pointer's allocated memory.
        this->~base_dyarray();
        m_pbuffer = Malloc::allocate(m_malloc, m_ncapacity);
        m_nlength = 0;
        m_iterator.set_range(m_pbuffer);
        return derived_cast();
    }
};
