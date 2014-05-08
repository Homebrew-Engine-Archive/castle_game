#include "point.h"
#include <cmath>
#include <iostream>

#include <SDL.h>

namespace
{

    template<class T>
    inline T Sqr(T x)
    {
        return x * x;
    }
    
}

Point::Point(int x, int y)
    : x(x)
    , y(y)
{ }

Point::Point()
    : Point{0, 0}
{ }

Point::operator SDL_Point() const
{
    return SDL_Point { x, y };
}

std::ostream &operator<<(std::ostream &out, const Point &pt)
{
    out << '(' << pt.x << ',' << pt.y << ')';
    return out;
}

int ManhattanDist(const Point &lhs, const Point &rhs)
{
    return Sqr(lhs.x - rhs.x) + Sqr(rhs.y - lhs.y);
}

double EuclidianDist(const Point &lhs, const Point &rhs)
{
    return hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

bool operator==(const Point &lhs, const Point &rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

bool operator!=(const Point &lhs, const Point &rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

bool operator<(const Point &lhs, const Point &rhs)
{
    return (lhs.x < rhs.x) || (lhs.x == rhs.x && lhs.y < rhs.y);
}

bool operator<=(const Point &lhs, const Point &rhs)
{
    return (lhs.x <= rhs.x) || (lhs.x == rhs.x && lhs.y <= rhs.y);
}

bool operator>(const Point &lhs, const Point &rhs)
{
    return (lhs.x > rhs.x) || (lhs.x == rhs.x && lhs.y > rhs.y);
}

bool operator>=(const Point &lhs, const Point &rhs)
{
    return (lhs.x >= rhs.x) || (lhs.x == rhs.x && lhs.y >= rhs.y);
}

Point& Point::operator+=(const Point &that)
{
    x += that.x;
    y += that.y;
    return *this;
}

Point& Point::operator-=(const Point &that)
{
    x -= that.x;
    y -= that.y;
    return *this;
}

Point operator-(const Point &lhs, const Point &rhs)
{
    return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}

Point operator+(const Point &lhs, const Point &rhs)
{
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point operator*(const Point &lhs, const Point &rhs)
{
    return Point(lhs.x * rhs.x, lhs.y * rhs.y);
}

Point operator/(const Point &lhs, const Point &rhs)
{
    return Point(lhs.x / rhs.x, lhs.y / rhs.y);
}

Point operator-(const Point &pt, int x)
{
    return Point(pt.x - x, pt.y - x);
}

Point operator+(const Point &pt, int x)
{
    return Point(pt.x + x, pt.y + x);
}

Point operator*(const Point &pt, int x)
{
    return Point(pt.x * x, pt.y * x);
}

Point operator/(const Point &pt, int x)
{
    return Point(pt.x / x, pt.y / x);
}
