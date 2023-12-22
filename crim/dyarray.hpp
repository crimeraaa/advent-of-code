#pragma once

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdexcept>

namespace crim {
    /**
     * Growable 1D array of a certain type, and of that one type only.
     * @tparam T Known fundamental or user-defined datatype.
     * @tparam length How many elements to start the internal buffer with.
     * @note For 2D arrays and/or greater dimensions, you may want to consider
     * implementing a matrix yourself as the cache lookups here may be slow.
     */
    template<typename T> class dyarray {
        static constexpr size_t ARRAY_GROWBUF = 16;
        static constexpr size_t ARRAY_MAXSIZE = 0xFFFF;
    protected:
        size_t m_size;
        size_t m_capacity;
        T *m_buffer;
    public:
        /**
         * Need at least 1 element which is the nul char by default
         * @exception `std::bad_alloc` from memory allocation via `new` operator.
         */
        dyarray() 
            : m_size{0}
            , m_capacity{ARRAY_GROWBUF}
            , m_buffer{new T[ARRAY_GROWBUF]} // if 0, will still get a valid handle
        {
            // printf("[CREATE] Making an instance of a dyarray...\n");
        }

        // Copy-constructor
        // Caller instance may be destroyed after this!
        dyarray(const dyarray &source) 
            : m_size{source.m_size} 
            , m_capacity{source.m_capacity}
            , m_buffer{new T[source.m_capacity]}
        {
            // printf("[COPY]: Making a copy of a dyarray...\n");
            memcpy((void*)m_buffer, (void*)source.m_buffer, sizeof(T) * m_size);
        }

        // Move-constructor: https://en.cppreference.com/w/cpp/language/move_constructor
        // dyarray(dyarray &&source) noexcept = default;

        // Careful when returning copy-by-value of this container,
        // as copies will have the same pointer as the original and both will
        // point to freed memory.
        ~dyarray() {
            printf("[DELETE] \"%s\"...\n", (char*)m_buffer);
            delete[] m_buffer;
        }

        // virtual void destroy() {
        //     printf("[DELETE] \"%s\"...\n", (char*)m_buffer);
        //     delete[] m_buffer;
        // }

        // ------------ ACCESS TO BUFFER INFORMATION FUNCTIONS ---------------------

        // How many elements we can hold.
        size_t capacity() {
            return m_capacity;
        }

        // How many elements we're actually holding currently.
        size_t size() {
            return m_size;
        }

        // 
        bool empty() {
            return m_size == 0;
        }

        // Our hardcoded/arbitrary maximum so that we don't carelessly allocate any
        // more memory a certain point.
        static constexpr size_t max_size() {
            return ARRAY_MAXSIZE;
        }

        // ------------ MANIPULATION OF BUFFER ELEMENTS FUNCTIONS ------------------

        /**
         * Read-write access to a particular element of the array's internal buffer.
         * @exception `std::out_of_range` if requested index exceeds `m_size`.
         * @note [See here](https://stackoverflow.com/a/30680219) if throwing is a good idea.
         * @note Unhandled exceptions simply make the program terminate.
         */
        T &at(size_t index) {
            if (index > m_size) {
                throw std::out_of_range("Out of range index into buffer!");
            }
            return m_buffer[index];
        }

        /**
         * Read-write access to a particular element of the array's internal buffer.
         * @exception `std::out_of_range` if requested index exceeds `m_size`.
         * @note [See here](https://stackoverflow.com/a/30680219) if throwing is a good idea.
         * @note Unhandled exceptions simply make the program terminate.
         */
        T &operator[] (size_t index) {
            return this->at(index);
        }

        /**
         * Reallocates memory for new block of `size` and copies previous data.
         * @note Like C's realloc, you have to reassign the target yourself.
         * @exception `std::bad_alloc` from memory allocation via `new` operator.
         */
        T *resize(size_t size) {
            T *tmp = new T[size];
            // Copy previous data only up to currently written point
            for (size_t i = 0; i < m_size; i++) {
                tmp[i] = m_buffer[i];
            }
            delete[] m_buffer;
            return tmp;
        }

        /**
         * Attempts to append `element` to the back of the internal buffer.
         * @exception `std::length_error` 
         */
        void push_back(T element) {
            // Check for this first juuuuuust in case
            if (m_size == ARRAY_MAXSIZE) {
                throw std::length_error("Reached buffer maximum allocations!");
            } else if (m_size + 1 > m_capacity) {
                // m_capacity += ARRAY_GROWBUF;
                m_capacity *= 2; // if it didn't start at 0 or 1 we gucci
                m_buffer = resize(m_capacity);
            } 
            m_buffer[m_size++] = element;
        }

        T pop_back() {
            return m_buffer[m_size--];
        }

        /**
         * `delete`s `m_buffer`, resets `m_size` to 0 and creates a new, fresh
         * block buffer of size `m_capacity`. Does not affect `m_capacity`.
         * 
         * @note This also makes it so that if the instance is immediately 
         * deleted after this, we won't free already freed memory.
         */
        void clear() {
            m_size = 0;
            delete[] m_buffer;
            m_buffer = new T[m_capacity];
        }

        // ------------------------ ITERATION ----------------------------------

        T *begin() {
            return &m_buffer[0];
        }

        T *end() {
            return &(m_buffer[m_size] + 1);
        }
    };
};
