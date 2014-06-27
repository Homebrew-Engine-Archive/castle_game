#ifndef INTERVAL_H_
#define INTERVAL_H_

#include <iostream>

#include <core/minmax.h>

namespace core
{
    /**
       Simple closed interval data type
    **/
    
    template<class T>
    struct Interval
    {
        T min;
        T max;

        explicit constexpr Interval();
        explicit constexpr Interval(T mn, T mx);
    };

    template<class T>
    constexpr Interval<T>::Interval()
        : Interval(T(), T())
    {
    }

    template<class T>
    constexpr Interval<T>::Interval(T mini, T maxi)
        : min(mini)
        , max(maxi)
    {
    }
    
    template<class T>
    constexpr bool IntervalEmpty(const Interval<T> &lhs)
    {
        return lhs.max <= lhs.min;
    }

    template<class T>
    constexpr const Interval<T> Intersection(const Interval<T> &lhs, const Interval<T> &rhs)
    {
        return Interval<T>(Max(lhs.min, rhs.min), Min(lhs.max, rhs.max));
    }

    template<class T>
    constexpr const Interval<T> Union(const Interval<T> &lhs, const Interval<T> &rhs)
    {
        return Interval<T>(Min(lhs.min, rhs.min), Max(lhs.max, rhs.max));
    }

    template<class T>
    constexpr bool Intersects(const Interval<T> &lhs, const Interval<T> &rhs)
    {
        return !IntervalEmpty(Intersection(lhs, rhs));
    }

    template<class T>
    constexpr bool operator==(const Interval<T> &lhs, const Interval<T> &rhs)
    {
        return (lhs.min == rhs.min) && (lhs.max == rhs.max);
    }

    template<class T>
    constexpr bool operator!=(const Interval<T> &lhs, const Interval<T> &rhs)
    {
        return (lhs.min != rhs.min) || (lhs.max != rhs.max);
    }

    template<class T>
    std::ostream& operator<<(std::ostream &out, const Interval<T> &range)
    {
        out << '[' << range.min << ',' << range.max << ']';
        return out;
    }
}

#endif // RANGE_H_
