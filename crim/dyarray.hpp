#include "base_dyarray.hpp"

namespace crim {
    template<class ElemT> class dyarray;
};

/**
 * @brief   Programmer-facing version of `crim::base_dyarray`. You're meant 
 *          to use this with fundamental/user-defined types only.
 * 
 * @tparam  ElemT   Desired type of the buffer's elements.
 * 
 * @warning This is a work in progress!
 */
template<class ElemT> 
class crim::dyarray : public crim::base_dyarray<crim::dyarray<ElemT>, ElemT> {
private:
    using base_class = base_dyarray<dyarray<ElemT>, ElemT>;
public:
    // Delegate's to base class's constructor for no arguments.
    // Does some basic allocations.
    dyarray() : base_class()
    {}

    // Delegates to base class's constructor for initializer lists.
    // Think of them like array literals.
    dyarray(std::initializer_list<ElemT> list) : base_class(list)
    {}
};
