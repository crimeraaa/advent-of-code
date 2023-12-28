/*- -*- ---***--- -*-        THE C STANDARD LIBRARY       -*- ---***---  -*- -*/
#include <stdio.h>


/*- -*- ---***--- -*-       THE C++ STANDARD LIBRARY      -*- ---***---  -*- -*/
#include <algorithm>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
/*- -*- ---***--- -*-                                     -*- ---***---  -*- -*/

template<typename ElemT> struct my_iterator {
    ElemT *m_pbegin;
    ElemT *m_pend;

    my_iterator(ElemT *start, ElemT *stop) 
    : m_pbegin{start}
    , m_pend{stop}
    {}

    my_iterator(ElemT *start, size_t count) 
    : my_iterator(start, start + count)
    {}

    my_iterator(my_iterator &src) 
    : my_iterator(src.begin(), src.end()) 
    {}

    void set_range(ElemT *start, ElemT *stop) {
        m_pbegin = start;
        m_pend = stop;
    }

    void set_range_n(ElemT *start, size_t count) {
        m_pbegin = start;
        m_pend = start + count;
    }

    bool empty() const {
        return m_pbegin != m_pend;
    }

    /* -*- ---***--- -*- ITERATOR POINTER ACCESS METHODS -*- ---***--- -*- */

    ElemT *begin() { return m_pbegin; }
    ElemT *end()   { return m_pend;   }

    const ElemT *begin() const { return m_pbegin; }
    const ElemT *end()   const { return m_pend;   }
};

template<typename ElemT, class AllocT = std::allocator<ElemT>> class my_array {
protected:
    using Mallocator = std::allocator_traits<AllocT>;
    AllocT m_malloc;
    size_t m_nlength; // number of elements currently written to the buffer.
    size_t m_ncapacity; // total number of elements buffer can hold currently.
    ElemT *m_pbuffer;
    my_iterator<ElemT> m_iterator;

public:
    my_array(size_t n_length, size_t n_capacity)
    : m_malloc{}
    , m_nlength{n_length}
    , m_ncapacity{n_capacity}
    , m_pbuffer{Mallocator::allocate(m_malloc, m_ncapacity)}
    , m_iterator(m_pbuffer, m_nlength) 
    {}

    my_array() : my_array(0, 0) {}

    // Should work for both const and non-const instances.
    my_array(std::initializer_list<ElemT> list) : my_array(list.size(), list.size()) {
        // Should call the non-const version even if our instance is const.
        std::copy(list.begin(), list.end(), m_iterator.begin());
    }

    // Copy-constructor.
    my_array(const my_array &src) : my_array(src.length(), src.capacity()) {
        // Should call the non-const version even if our instance is const.
        std::copy(src.begin(), src.end(), m_iterator.begin());
    }

    my_array(my_array &&src) 
    : m_malloc{src.m_malloc}
    , m_nlength{src.m_nlength}
    , m_ncapacity{src.m_ncapacity}
    , m_pbuffer{src.m_pbuffer}
    , m_iterator(m_pbuffer, m_nlength) {
        // Erase so that upon rvalue `src`'s destruct the memory isn't freed.
        src.m_pbuffer = nullptr;
    }

    /**
     * @brief   For elements with non-trivial destructors, we really need to 
     *          explicitly call `Mallocator::destroy()`! 
     * 
     *          Mainly in the case of heap-allocated container instances, like
     *          `std::string`, no longer subject to small-string optimization.
     * 
     * @note    C++17 and above only:
     *          `std::destroy_n(m_pbuffer, m_ncapacity);`
     */
    ~my_array() {
        for (size_t i = 0; i < m_ncapacity; i++) {
            Mallocator::destroy(m_malloc, m_pbuffer + i);
        }
        Mallocator::deallocate(m_malloc, m_pbuffer, m_ncapacity);
    }

    /* -*- ---***--- -*- NON-CONST DATA ACCESS METHODS -*- ---***--- -*- */

    ElemT &at(size_t index) {
        return m_pbuffer[index];
    }

    ElemT &operator[](size_t index) {
        return at(index);
    }

    /* -*- ---***--- -*- CONST DATA ACCESS METHODS -*- ---***--- -*- */

    size_t length() const {
        return m_nlength;
    }

    size_t capacity() const {
        return m_ncapacity;
    }

    const ElemT *data() const {
        return m_pbuffer;
    }

    const ElemT &at(size_t index) const {
        return m_pbuffer[index];
    }

    const ElemT &operator[](size_t index) const {
        return at(index);
    }

    /* -*- ---***--- -*- ITERATORS -*- ---***--- -*- */

    // Readable and writeable pointer to the first element of the array.
    ElemT *begin() { 
        return m_iterator.begin(); 
    } 

    ElemT *end() { 
        return m_iterator.end();   
    }

    // Read-only pointer to the first element of the array.
    const ElemT *begin() const { 
        return m_iterator.begin(); 
    }

    // Read-only pointer 1 address past the end of the array. Don't dereference!
    const ElemT *end() const { 
        return m_iterator.end();   
    }

    /* -*- ---***--- -*- BUFFER MANIPULATION -*- ---***--- -*- */

    void push_back(ElemT &&entry) {
        check_size();
        // This block of memory must be already initialized for this to work!
        m_pbuffer[m_nlength++] = std::move(entry); 
    }

    void push_back(const ElemT &entry) {
        check_size();   
        m_pbuffer[m_nlength++] = entry; // call copy-assignment
    }

private:
    void check_size() {
        // Try to grow buffer if no more allocated space
        if (m_nlength + 1 > m_ncapacity) {
            // Can't multiply 0 by 1 to fallback to 1, else do multiply by 2.
            size_t n_newcap = (m_ncapacity == 0) ? 16 : m_ncapacity * 2;
            resize(n_newcap);
        }
        m_iterator.m_pend++; // will point to 1 past element to be written
    }

public:
    // Since it's likely m_nlength was post-incremented for the next element,
    // use pre-decrement to get the current latest element.
    ElemT pop_back() {
        m_iterator.m_pend--;
        return m_pbuffer[--m_nlength];
    }

    void resize(size_t n_newcap) {
        // This branch is stupid so just get out of here ASAP
        if (n_newcap == m_ncapacity) {
            return;
        }

        ElemT *p_dummy = Mallocator::allocate(m_malloc, n_newcap);

        // Move semantics prolly prevents deep copies of heap-alloc'd memory
        // std::uninitialized_move_n(m_pbuffer, m_nlength, p_dummy);
        for (size_t i = 0; i < length(); i++) {
            p_dummy[i] = std::move(m_pbuffer[i]);
        }

        /**
         * @brief   Slight optimization where we only default construct unmoved 
         *          memory. That is, there is likely a portion of memory alloc'd
         *          for `p_dummy` but not touched by the above loop.
         * 
         *          Since std::allocator returns unitialized memory, we must
         *          initialize it ourselves so that we can copy/move-assign 
         *          non-trivial types later on.
         * 
         * @note    You could use this instead:
         *          `std::uninitialized_default_construct_n(startptr, length)`;
         */
        for (size_t i = length(); i < n_newcap; i++) {
            Mallocator::construct(m_malloc, p_dummy + i);
        }

        // Get rid of old memory as don't need it anymore, then switch pointers.
        Mallocator::deallocate(m_malloc, m_pbuffer, m_nlength);

        m_ncapacity = n_newcap;
        m_pbuffer = p_dummy;
        m_iterator.set_range_n(m_pbuffer, m_nlength);
    }
};

void int_array_test() {
    const my_array<int> t3 = {1, 2, 3, 4};
    auto t4 = t3;
    printf("t3\n");
    for (auto ptr = t3.begin(); ptr < t3.end(); ptr++) {
        printf("my_array[%td] %i (%p)\n", ptr - t3.begin(), *ptr, (void*)ptr);
    }
    printf("\nt4\n");
    for (auto ptr = t4.begin(); ptr < t4.end(); ptr++) {
        // %t indicates the specifier is for some kind of ptrdiff_t.
        // See: https://stackoverflow.com/a/19684560
        printf("my_array[%td] %i (%p)\n", ptr - t4.begin(), *ptr, (void*)ptr);
    }
}

void std_vector_string_test() {
    std::vector<std::string> vec_s;
    vec_s.push_back("Hi mom!");
    vec_s.push_back("Hello there!");
    for (const auto &s : vec_s) {
        printf("%s\n", s.c_str());    
    }
    std::string a = "aaa";
    std::string b = "bbb";
    a = std::move(b);
}

void my_array_string_test() {
#ifdef CRIM_ALLOCATOR_TEST_INITIALIZER_LIST
    my_array<std::string> arr_s = {
        "Hi mom!", 
        "Hello there!", 
        "This is a very long string to see if non-small strings work.", 
        "nope",
    };
#else
    my_array<std::string> arr_s;
    arr_s.push_back("Hi mom!");
    arr_s.push_back("Hello there!");
    arr_s.push_back("A very long string to see if non-small strings work.");
    arr_s.push_back("nope");
#endif
    for (const auto &s : arr_s) {
        printf("%s\n", s.c_str());
    }
    for (size_t i = 0; i < arr_s.length(); i++) {
        printf("%zu: \"%s\"\n", i, arr_s.at(i).c_str());
    }
}

// No memory leaks with fundamental types, so it must be how I'm handling 
// types with constructors/destructors and heap-allocated pointers.
void my_array_int_test() {
    my_array<int> arr = {121, 131, 9911, 0132, 1111, 0xFFFF, 0b101};
    arr.push_back(14);
    arr.push_back(19);
    arr.push_back(171);
    for (const auto &i : arr) {
        printf("%i\n", i);
    }
}

int main() {
    // std_vector_string_test();
    my_array_string_test();
    my_array_int_test();
    return 0;
}
