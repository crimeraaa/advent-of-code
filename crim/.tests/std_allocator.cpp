#include <algorithm>
#include <memory>
#include <string>
#include <iostream>

#define ElemT std::string

class dummy_array {
protected:
    using Allocator= std::allocator<ElemT>;
    using AllocTraits = std::allocator_traits<Allocator>;
    Allocator m_malloc;
    size_t m_nlength;
    size_t m_ncapacity;
    ElemT *m_pbuffer;
public:
    dummy_array()
    : m_malloc{}
    , m_nlength{0}
    , m_ncapacity{0}
    , m_pbuffer{nullptr}
    {}

    ~dummy_array() {
        m_malloc.deallocate(m_pbuffer, m_ncapacity);
    }

    void push_back(ElemT &&entry) {
        checksize();
        m_pbuffer[m_nlength++] = std::move(entry);
    }

    void push_back(const ElemT &entry) {
        checksize();
        m_pbuffer[m_nlength++] = entry;
    }

    void checksize() {
        if (m_nlength + 1 > m_ncapacity) {
            resize((m_ncapacity == 0) ? 16 : m_ncapacity * 2);
        }
    }

    void resize(size_t n_newcap) {
        if (n_newcap == m_ncapacity) {
            return;
        }

        ElemT *p_dummy = m_malloc.allocate(n_newcap, m_pbuffer);
        // Default initialize our uninitialized memory so we can poke at it
        std::uninitialized_default_construct_n(p_dummy, n_newcap);

        // Should be safe to poke around now!
        std::uninitialized_move_n(m_pbuffer, m_nlength, p_dummy);

        // new_allocator.h says: "__p is not permitted to be a null pointer."
        if (m_pbuffer != NULL && m_ncapacity != 0) {
            m_malloc.deallocate(m_pbuffer, m_ncapacity);
        }
        m_pbuffer = p_dummy;
        m_ncapacity = n_newcap;
    }
};

// Generic print items one after the other function
template<typename TestT> void print_item(const TestT &item) {
    std::cout << item << ", ";
}

// Specialization for strings
template<> void print_item<std::string>(const std::string &item) {
    std::cout << "\"" << item.c_str() << "\", ";
}

template<typename TestT>
void print_array(const char *msg, const TestT *begin, size_t count) {
    const auto *end = begin + count;
    std::cout << msg << ": ";
    while (begin < end) {
        print_item<TestT>(*begin);
        begin++;
    }
    printf("\n");
}

void new_and_delete() {
    const size_t items = 4;
    auto *p_in = new std::string[items];
    auto *p_out = new std::string[items];

    p_in[0] = "Hi mom!";
    p_in[1] = "Hello there!";
    p_in[2] = "This is a really long string that should not benefit from SSO.";
    p_in[3] = "I am the globgogabgalab!";

    print_array("before move {in}: ", p_in, items);
    print_array("before move {out}: ", p_out, items);

    // in + items is effectively the address &in[2], which is out of bounds.
    std::move(p_in, p_in + items, p_out);

    print_array("after move {in}:  ", p_in, items);
    print_array("after move {out}: ", p_out, items);

    delete[] p_in;
    delete[] p_out;
}

void mallocator_strings() {
    const size_t n_items = 4;
    using Mallocator = std::allocator<std::string>;
    using MallocTraits = std::allocator_traits<Mallocator>; // jfc...
    Mallocator m_malloc;

    // Allocates uninitialized memory for us, we likely have to initialize it
    // ourselves in order to use it. This is especially important for classes.
    auto *p_in = m_malloc.allocate(n_items);
    auto *p_out = m_malloc.allocate(n_items);

    // This is crucial! If using the std allocator, we gotta do this explicitly.
    // Also, the pointer arithmetic gets the correct addresses for us already.
    for (size_t i = 0; i < n_items; i++) {
        MallocTraits::construct(m_malloc, p_in + i);
        MallocTraits::construct(m_malloc, p_out + i);
    }

    // Otherwise, valgrind will slap us for overwriting uninitialized memory
    // which is then lost due to these constructors/assignments.
    p_in[0] = "Hi mom!";
    p_in[1] = "Hello there!";
    p_in[2] = "I am the globgogabgalab!";

    // This one leaks memory if we don't explicitly call AllocTraits::destroy().
    p_in[3] = "This is a really long string that should not benefit from SSO.";


    print_array("before move {in}:  ", p_in, n_items);
    print_array("before move {out}: ", p_out, n_items);

    std::move(p_in, p_in + n_items, p_out);

    print_array("after move {in}:   ", p_in, n_items);
    print_array("after move {out}:  ", p_out, n_items);

    // This works!
    for (size_t i = 0; i < n_items; i++) {
        MallocTraits::destroy(m_malloc, p_in + i);
        MallocTraits::destroy(m_malloc, p_out + i);
    }

    m_malloc.deallocate(p_in, n_items);
    m_malloc.deallocate(p_out, n_items);
}

// `int` is a fundamental type, it's "moved" (in this case, "copied") by value.
void mallocator_ints() {
    const size_t n_items = 2;
    using Mallocator = std::allocator<int>;
    using MallocTraits = std::allocator_traits<Mallocator>;
    Mallocator m_malloc;

    auto *p_in = m_malloc.allocate(n_items);
    auto *p_out = m_malloc.allocate(n_items);
    for (size_t i = 0; i < n_items; i++) {
        MallocTraits::construct(m_malloc, p_in + i);
        MallocTraits::construct(m_malloc, p_out + i);
    }

    0[p_in] = 14; // lol this is valid due to how C resolves array indexing
    1[p_in] = 16; // cause it's really just pointer arithmetic!

    print_array("before move {in}:  ", p_in, n_items);
    print_array("before move {out}: ", p_out, n_items);

    std::move(p_in, p_in + n_items, p_out);

    print_array("after move {in}:   ", p_in, n_items);
    print_array("after move {out}:  ", p_out, n_items);

    for (size_t i = 0; i < n_items; i++) {
        MallocTraits::destroy(m_malloc, p_in + i);
        MallocTraits::destroy(m_malloc, p_out + i);
    }

    m_malloc.deallocate(p_in, n_items);
    m_malloc.deallocate(p_out, n_items);
}

// https://en.cppreference.com/w/cpp/memory/uninitialized_move
void mallocator_unitialized_move_strings() {
    const size_t n_items = 4;
    using Mallocator = std::allocator<std::string>;
    // using MallocTraits = std::allocator_traits<Mallocator>; // jfc...
    Mallocator m_malloc;

    auto *p_in = m_malloc.allocate(n_items);
    auto *p_out = m_malloc.allocate(n_items);

    std::uninitialized_default_construct(p_in, p_in + n_items);
    
    p_in[0] = "Hi mom!";
    p_in[1] = "Hello there!";
    p_in[2] = "I am the globgogabgalab!";

    // This one leaks memory if we don't explicitly call AllocTraits::destroy().
    p_in[3] = "This is a really long string that should not benefit from SSO.";

    print_array("before move {in}:  ", p_in, n_items);
    // Don't do this since they're uninitialized!
    // print_array("before move {out}: ", p_out, n_items);

    std::uninitialized_move(p_in, p_in + n_items, p_out);

    print_array("after move {in}:   ", p_in, n_items);
    print_array("after move {out}:  ", p_out, n_items);

    std::destroy(p_out, p_out + n_items);

    m_malloc.deallocate(p_in, n_items);
    m_malloc.deallocate(p_out, n_items);
}

int main() {
    // dummy_array d;
    // d.push_back("Hi mom!");
    // std::string in[] = {"Hi mom!", "Hello there!"};
    // new_and_delete();
    // mallocator_strings();
    // mallocator_ints();
    mallocator_unitialized_move_strings();
    return 0;
}
