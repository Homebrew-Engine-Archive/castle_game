#include "interval.h"

namespace
{
    void test()
    {
        using namespace core;
        constexpr const Interval<int> a = Interval<int>(-1,-1);
        constexpr const Interval<int> b = Interval<int>(0,3);
        constexpr const Interval<int> c = Interval<int>(7,7);
        
        static_assert(IntervalEmpty(a), "a is empty");
        static_assert(!IntervalEmpty(b), "b is not empty");
        static_assert(IntervalEmpty(c), "c is empty");
        
        static_assert(IntervalEmpty(Intersection(a, b)), "intersection a and b is empty");
        static_assert(IntervalEmpty(Intersection(a, c)), "intersection a and c is empty");
        static_assert(IntervalEmpty(Intersection(b, c)), "intersection b and c is empty");
    }
}
