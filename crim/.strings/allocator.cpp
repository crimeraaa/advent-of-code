/* MinGW doesn't support %zu: https://stackoverflow.com/a/52383920 */
#if defined(__MINGW32__) || defined(__MINGW64__)
#define __USE_MINGW_ANSI_STDIO 1
#endif

#include <cstdio>
#include <string>
#include "allocator.tcc"

class genarray {
private:
    using ElemT = std::string;
    using AllocT = crim::allocator<ElemT>;
private:
    AllocT m_malloc;
    size_t m_ncount;
    size_t m_ncapacity;
    ElemT *m_pbuffer;
public:
    genarray(size_t m_nstartcap = 0) 
    : m_malloc{}
    , m_ncount{0}
    , m_ncapacity{m_nstartcap}
    , m_pbuffer{m_malloc.allocate(m_nstartcap)} {}
    
    ~genarray() {
        for (size_t i = 0; i < m_ncount; i++) {
            m_malloc.destroy_at(&m_pbuffer[i]);
        }
        m_malloc.deallocate(m_pbuffer, m_ncapacity);
    }
    
    void print() {
        for (size_t i = 0; i < m_ncount; i++) {
            std::printf("genarray[%zu]: %s\n", i, m_pbuffer[i].c_str());
        }
        std::printf("\n");
    }
    
    void test() {
        for (size_t i = 0; i < m_ncapacity; i++) {
            m_malloc.construct_at(&m_pbuffer[m_ncount++], "0123456789abcdef");
        }
    }
};

int main() {
    genarray ga = genarray(16);
    ga.test();
    ga.print();
    return 0;
}
