namespace crim {
    /**
     * @brief       An iterator class for use with range-based for loops.
     * 
     *              For your own classes, you'll likely have to provide the 
     *              `begin()` and `end()` methods yourself.
     * 
     * @tparam      ElemT   Desired type of the caller's buffer.
     * 
     * @note        Valgrind suggests that using an out of bounds memory address
     *              for `m_end` is a bad idea, so be careful how you update it!
     *              Instead of dereferencing an index, use pointer arithmetic.
     */
    template<class ElemT> struct iterator {
        ElemT *m_begin; // Address of first written element.
        ElemT *m_end; // Address right past the last written element.

        /**
         * @brief   Primary constructor where you already know the starting and 
         *          ending addresses for your iterators.
         */
        iterator(ElemT *start, ElemT *stop) 
        : m_begin{start}, m_end{stop} {}

        /**
         * @brief   Secondary constructor where you know the starting address, 
         *          but you only know the number of elements allocated for this
         *          pointer (or you're too lazy to calculate).
         */
        iterator(ElemT *start, size_t count) 
        : iterator(start, start + count) {}

        /**
         * @brief   Copy-constructor.
         */
        iterator(const iterator &src) 
        : iterator(src.begin(), src.end()) {}

        /**
         * @brief   Move-constructor.
         */
        iterator(iterator &&src) 
        : iterator(src.begin(), src.end()) {}

        /**
         * @brief   If you have a new address and a new number of elements, use 
         *          this to modify an existing iterator.
         */
        void set_range(ElemT *start, size_t count = 0) {
            m_begin = start;
            m_end = start + count;
        }

        iterator &copy(const iterator &src) {
            m_begin = src.begin();
            m_end = src.end();
            return *this;
        }

        // Copy-assignment.
        iterator &operator=(const iterator &src) {
            return copy(src);
        }

        /**
         * @brief   Move-assignment.
         * 
         * @note    `src`, being named, is treated as if it were an lvalue 
         *          reference (a.k.a. `const iterator &`) when passed to other
         *          functions as-is. Think of it like "decaying".
         */
        iterator &operator=(const iterator &&src) {
            return copy(src);
        }

        /* -*- ---***--- -*- ITERATOR POINTER ACCESS METHODS -*- ---***--- -*- */
        // const functions so both const and non-const instances can use these.

        ElemT *begin() const { 
            return m_begin; 
        }
        
        ElemT *end() const { 
            return m_end;
        }

        bool empty() const { 
            return m_begin == m_end; 
        }
    };
}
