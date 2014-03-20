#include "point.h"
#include <cmath>
#include <iostream>

namespace
{

    template<class T>
    inline T Sqr(T x)
    {
        return x * x;
    }
    
}

std::ostream &operator<<(std::ostream &out, const Point &pt)
{
    out << '(' << pt.x << ',' << pt.y << ')';
    return out;
}

int SqrEuclidianDist(const Point &lhs, const Point &rhs)
{
    return Sqr(lhs.x - rhs.x) + Sqr(rhs.y - lhs.y);
}

double EuclidianDist(const Point &lhs, const Point &rhs)
{
    return sqrt(SqrEuclidianDist(lhs, rhs));
}

Point::Point(int x, int y)
    : x(x)
    , y(y)
{ }

Point::Point()
    : Point{0, 0}
{ }

bool Point::operator==(const Point &that) const
{
    return (that.x == x) && (that.y == y);
}


bool Point::operator!=(const Point &that) const
{
    return (that.x != x) || (that.y != y);
}

bool Point::operator<(const Point &that) const
{
    return (that.x < x) || ((that.x == x) && (that.y < y));
}

bool Point::operator<=(const Point &that) const
{
    return (that.x <= x) || ((that.x == x) && (that.y <= y));
}

Point Point::operator+(const Point &that) const
{
    return Point(x + that.x, y + that.y);
}

Point Point::operator-(const Point &that) const
{
    return Point(x - that.x, y - that.y);
}

Point &Point::operator+=(const Point &that)
{
    x += that.x;
    y += that.y;
    return *this;
}

Point &Point::operator-=(const Point &that)
{
    x -= that.x;
    y -= that.y;
    return *this;
}
