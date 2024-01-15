#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <stdexcept>
#include <new>

namespace crim {
    using msglit = const char *;
    int logerror(msglit file, int line, msglit scope, msglit func, msglit info) {
        return std::fprintf(stderr, 
            "%s:%i:\n"
            "\t%s::%s(): %s\n", 
            file, line, 
            scope, func, info
        );
    }
};

#define logerror(info) logerror(__FILE__, __LINE__, "crim", __func__, info)

namespace crim {
    // TODO: Turn into a templated nightmare :)
    struct string {
    private:
        using ElemT = char;
        // Anonymous unions expose their members to the enclosing scope.
        union {
            struct {
                ElemT *m_pbuffer;
                size_t m_ncapacity;
            } m_heapbuf;
            // `sizeof(m_heapbuf)` won't work when in a template so yeah.
            ElemT m_stackbuf[sizeof(m_heapbuf)] = {0};
        };
        size_t m_ncount = 0;

        enum BufInfo {
            stackbuf_cap = sizeof(m_stackbuf),
            stackbuf_end = stackbuf_cap - 1,
            neapbuf_start = stackbuf_cap,
            heapbuf_cap = stackbuf_cap * 2
        };
        
        const string *const_this() {
            return static_cast<const string *>(this);
        }
    public: 
        string() {}

        string(const string &other) {
            copy_buffer(other);
        }
        
        string(string &&rvalue) {
            move_buffer(std::move(rvalue)); // "decays" to lvalue ref otherwise
        }
        
        ~string() {
            if (isshort()) {
                return;
            }
            std::free(m_heapbuf.m_pbuffer);
        }

    private:
        void copy_buffer(const string &r_src) {
            if (r_src.isshort()) {
                for (size_t i = 0; i < r_src.m_ncount; i++) {
                    m_stackbuf[i] = r_src.m_stackbuf[i];        
                }
            } else {
                logerror("TODO: Make work with heapbuf.");
                throw std::runtime_error("See log message.");
            }
        }
        
        void move_buffer(string &&rvalue) {
            if (rvalue.isshort()) {
                for (size_t i = 0; i < rvalue.m_ncount; i++) {
                    m_stackbuf[i] = std::move(rvalue.m_stackbuf[i]);
                }
            } else {
                logerror("TODO: Make work with heapbuf.");
                throw std::runtime_error("See log message.");
            }
        }

    public: 
        const ElemT &at(size_t n_index) const {
            if (n_index >= m_ncount) {
                logerror("Requested an invalid index!");
                throw std::out_of_range("See log message.");
            }
            return isshort() ? m_stackbuf[n_index] : m_heapbuf.m_pbuffer[n_index];
        }

        // We can cast `this` to const and uncast const from const methods'
        // return values because they weren't const to begin with.
        ElemT &at(size_t n_index) {
            return const_cast<ElemT&>(const_this()->at(n_index));
        }

        // Stupid but this should solve the off by 1 error we had,
        // Since m_ncount of 15 OR 16 are still short strings.
        bool isshort() const {
            return m_ncount <= BufInfo::stackbuf_cap;
        }
        
        size_t length() const {
            return m_ncount;
        }
        
        // Includes allocated capacity for the terminating nul character.
        size_t capacity() const {
            return isshort() ? sizeof(m_stackbuf) : m_heapbuf.m_ncapacity;
        }
    private:
        // TODO: maybe we can be a bit smart with this and use a local instance.
        bool init_heapbuf() {
            // Copy-by-value on the old data first.
            ElemT p_prevdata[sizeof(m_stackbuf)];
            memcpy(p_prevdata, m_stackbuf, sizeof(p_prevdata));

            // Careful: now assigning the active union members!
            m_heapbuf.m_ncapacity = BufInfo::heapbuf_cap;
            
            // Maybe wanna turn this into a template sometime.
            size_t n_alloc = sizeof(*p_prevdata) * m_heapbuf.m_ncapacity;
            m_heapbuf.m_pbuffer = static_cast<ElemT*>(std::malloc(n_alloc));

            // To be handled by caller, but do log the error
            if (m_heapbuf.m_pbuffer == nullptr) {
                logerror("std::malloc() returned a nullptr!");
                return false;
            }
            
            // TODO: Perhaps we can use placement new.
            memcpy(m_heapbuf.m_pbuffer, p_prevdata, sizeof(p_prevdata));
            return true;
        }
    public:
        bool resize(size_t n_newcap) {
            // Don't do anything so we don't waste our time.
            if (n_newcap == m_heapbuf.m_ncapacity) {
                return true;
            }

            // Shortening the buffer has a few quirks.
            if (n_newcap < m_heapbuf.m_ncapacity) {
                m_ncount = n_newcap;
            }

            // I do it this way because C++ is hell bent on casting hell.
            size_t n_alloc = sizeof(*m_heapbuf.m_pbuffer) * n_newcap;
            ElemT *p_dummy = static_cast<ElemT*>(std::malloc(n_alloc));
            if (p_dummy == nullptr) {
                logerror("std::malloc() returned a nullptr!");
                return false;
            }

            // TODO: Perhaps we can use placement new.
            for (size_t i = 0; i < m_ncount; i++) {
                p_dummy[i] = m_heapbuf.m_pbuffer[i];
            }

            // Switcharoo and cleanup.
            p_dummy[m_ncount] = '\0';
            std::free(m_heapbuf.m_pbuffer);
            m_heapbuf.m_pbuffer = p_dummy;
            m_heapbuf.m_ncapacity = n_newcap;
            return true;            
        }

        bool push_back(ElemT ch) {
            // Can't use isshort() due to off by 1, since once we need to write
            // to the 16th index we need to start the heapbuf.
            if (m_ncount < BufInfo::stackbuf_cap) {
                m_stackbuf[m_ncount++] = ch;
                return true;
            }

            // Initialize heapbuf exactly once.
            if (m_ncount == BufInfo::stackbuf_cap) {
                if (init_heapbuf() == false) {
                    logerror("crim::string::init_heapbuf() failed!");
                    return false;
                }
            }

            // Check for enough allocated memory, if not then try to resize!
            if (m_ncount + 1 >= m_heapbuf.m_ncapacity) {
                if (resize(m_heapbuf.m_ncapacity * 2) == false) {
                    logerror("crim::string::resize() failed!");
                    return false;
                }
            }
            m_heapbuf.m_pbuffer[m_ncount++] = ch;
            return true;
        }
        
        const ElemT *c_str() const {
            return isshort() ? m_stackbuf : m_heapbuf.m_pbuffer;
        }
    };

    bool isendl(int ch) {
        return ch == '\r' || ch == '\n';
    }
    
    bool readcrlf(std::FILE *p_stream, int ch) {
        if (ch == '\r' && (ch = fgetc(p_stream)) != EOF) {
            if (ch != '\n' && ungetc(ch, p_stream) == EOF) {
                logerror("Failed to read CR/CRLF line ending!");
                return false;
            }
        }
        return true;
    }
    
    string readline(std::FILE *p_stream) {
        string input;
        int ch;

        while ((ch = std::fgetc(p_stream)) != EOF && !isendl(ch)) {
            if (!input.push_back(static_cast<char>(ch))) {
                logerror("Failed to append normal character!");
                break;
            }    
        } 

        // Especially important for malloc'd unitialized memory.
        if (!input.push_back('\0')) {
            logerror("Failed to append nul character!");
        }
        
        if (!readcrlf(p_stream, ch)) {
            logerror("crim::readcrlf() failed!");
        }

        return input;
    }
    
    string get_string(const char *p_fmts, ...) {
        std::va_list p_args;
        va_start(p_args, p_fmts);
        std::vfprintf(stdout, p_fmts, p_args);
        va_end(p_args);
        return readline(stdin);
    }
};

#undef logerror
