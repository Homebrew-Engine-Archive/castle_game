#include "point.h"

#include <cmath>
#include <iostream>

#include <core/rect.h>

namespace
{
    void test()
    {
        constexpr core::Rect r(100, 100, 200, 200);

        constexpr core::Point a(0, 0);
        constexpr core::Point b(100, 100);
        constexpr core::Point c(99, 150);
        constexpr core::Point d(150, 99);
        
        static_assert(!core::PointInRect(r, a), "A in R");
        static_assert(core::PointInRect(r, b), "B not in R");
        static_assert(!core::PointInRect(r, c), "C in R");
        static_assert(!core::PointInRect(r, d), "D in R");
    }
    
    template<class T>
    constexpr T Sqr(T x)
    {
        return x * x;
    }
}

namespace core
{
    Point& Point::operator+=(const Point &that)
    {
        mX += that.mX;
        mY += that.mY;
        return *this;
    }

    Point& Point::operator-=(const Point &that)
    {
        mX -= that.mX;
        mY -= that.mY;
        return *this;
    }

    Point& Point::operator*=(const Point &that)
    {
        mX *= that.mX;
        mY *= that.mY;
        return *this;
    }

    Point& Point::operator/=(const Point &that)
    {
        mX /= that.mX;
        mY /= that.mY;
        return *this;
    }

    Point& Point::operator+=(Point::coord_type v)
    {
        mX += v;
        mY += v;
        return *this;
    }

    Point& Point::operator-=(Point::coord_type v)
    {
        mX -= v;
        mY -= v;
        return *this;
    }

    Point& Point::operator*=(Point::coord_type v)
    {
        mX *= v;
        mY *= v;
        return *this;
    }

    Point& Point::operator/=(Point::coord_type v)
    {
        mX /= v;
        mY /= v;
        return *this;
    }

    Point::coord_type Manhattan(const Point &lhs, const Point &rhs)
    {
        return abs(lhs.X() - rhs.X()) + abs(rhs.Y() - lhs.Y());
    }

    float Hypot(const Point &lhs, const Point &rhs)
    {
        return hypot(lhs.X() - rhs.X(), lhs.Y() - rhs.Y());
    }

    std::ostream& operator<<(std::ostream &out, const Point &pt)
    {
        out << '(' << pt.X() << ',' << pt.Y() << ')';
        return out;
    }
}
