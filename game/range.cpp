#include "range.h"

namespace
{
    void test()
    {
        using namespace core;
        constexpr const Range<int> a = Range<int>(-1,-1);
        constexpr const Range<int> b = Range<int>(0,3);
        constexpr const Range<int> c = Range<int>(7,7);
        
        static_assert(RangeEmpty(a), "a is empty");
        static_assert(!RangeEmpty(b), "b is not empty");
        static_assert(RangeEmpty(c), "c is empty");
        
        static_assert(RangeEmpty(Intersection(a, b)), "intersection a and b is empty");
        static_assert(RangeEmpty(Intersection(a, c)), "intersection a and c is empty");
        static_assert(RangeEmpty(Intersection(b, c)), "intersection b and c is empty");

    }
}
