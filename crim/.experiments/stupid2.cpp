// If you want to share implementations and preserve type information, 
// then CRTP (Continuous Recurring Template Pattern) is what you need!
// https://stackoverflow.com/a/56423659

#include <stdio.h>
#include <string.h>
#include <stdexcept>

// What's the empty struct for?
// struct BufferBase {};

/** 
 * This is our base class which descendents should inherit AND specialize from.
 * In other words, our base class allows for common implementation yet unique
 * types. 
 * @tparam TemplateT The target descendent class so we can return their references.
 * @tparam ElemT The target descendent class's buffer element type.
 */
template <class ClassT, typename ElemT> 
class Common {
private:
    friend ClassT; // We can access template parameter's member variables now!
    // BaseArray() = default; // Creates/uses compiler-generated default constructor

    // This one does the real work for returning our class's references!
    ClassT &cthis() {
        return static_cast<ClassT&>(*this);
    }
    // Overload for const references.
    ClassT &cthis() const {
        return static_cast<ClassT const&>(*this);
    }
protected:
    static constexpr int STARTCAPACITY = 16;
    size_t m_index;
    size_t m_capacity;
    ElemT *m_buffer;
public:
    Common() 
        : m_index{0}
        , m_capacity{STARTCAPACITY}
        , m_buffer{new ElemT[STARTCAPACITY]}
    {}

    Common(const Common &source) 
        : m_index{source.m_index}
        , m_capacity{source.m_capacity}
        , m_buffer{new ElemT[m_capacity]}
    {
        memcpy(m_buffer, source.m_buffer, sizeof(ElemT) * m_capacity);
    }

    ~Common() {
        delete[] m_buffer;
    }

    size_t size() const {
        return m_index;
    }

    size_t capacity() const {
        return m_capacity;
    }

    ElemT &at(size_t index) {
        if (index > m_index) {
            throw std::out_of_range("Attempt to access invalid index!");
        }
        return m_buffer[index];
    }

    ElemT &operator[](size_t index) {
        return at(index);
    }

    // Example of a common implementation!
    ClassT &push_back(ElemT entry) {
        printf("Called BaseArray::push_back()!\n");
        (void)entry;
        // if (m_index + 1 > m_capacity) {
        //     resize(m_capacity * 2);
        // }
        // m_buffer[m_index++] = entry;
        return cthis();
    }

    // For simplicity's sake only doing buffer growth/extension for now.
    ClassT &resize(size_t new_size) {
        printf("Called BaseArray::resize()!\n");
        (void)new_size;
        // ElemT *dummy = new ElemT[new_size];
        // memcpy(dummy, m_buffer, new_size);
        // delete[] m_buffer;
        // m_buffer = dummy;
        // m_capacity = new_size;
        return cthis();
    }
};

// Note how we pass Array as if it were templated into Common!
template<class ElemT> 
class Array : public Common<Array<ElemT>, ElemT> {
public:
    // Example of a specialized implementation!
    Array &reverse() {
        printf("Called Array::reverse()!\n");
        return *this;
    }

    Array &append() {
        printf("Called Array::append()!\n");
        return *this;
    }
};

int main() {
    Array<int> ca;
    ca.reverse().push_back(1).append().resize(1);
    return 0;
}
