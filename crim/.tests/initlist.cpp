#include <initializer_list>
#include <iostream>
#include <vector>
#include <cstdio>

template<class T> struct container {
    std::vector<T> m_vlist;
    container(std::initializer_list<T> list) : m_vlist(list) {
        std::printf("Constructed with a %zu-element list!\n", list.size());
    }

    container &append(std::initializer_list<T> list) {
        // This overload inserts a range using the input's iterators.
        m_vlist.insert(m_vlist.end(), list.begin(), list.end());
        return *this;
    }

    const T *begin() const {
        return &m_vlist.at(0);
    }

    const T *end() const {
        // Subtract 1 to avoid .at throwing an error, then add 1 to get 1 past.
        return &m_vlist.at(m_vlist.size() - 1) + 1;
    }

    bool empty() const {
        return begin() == end();
    }
};

int main() {
    std::initializer_list<int> list = {1, 2, 3, 4, 5};
    container<int> test = list; // copy-list initialization
    for (const auto &i : test) {
        printf("%i, ", i);
    }
    return 0;
}
