#include "point.h"

#include <cmath>
#include <iostream>

namespace
{
    template<class T>
    constexpr T Sqr(T x)
    {
        return x * x;
    }
}

Point& Point::operator=(const SDL_Point &that)
{
    x = that.x;
    y = that.y;
    return *this;
}

Point& Point::operator+=(const SDL_Point &that)
{
    x += that.x;
    y += that.y;
    return *this;
}

Point& Point::operator-=(const SDL_Point &that)
{
    x -= that.x;
    y -= that.y;
    return *this;
}

Point& Point::operator+=(int v)
{
    x += v;
    y += v;
    return *this;
}

Point& Point::operator-=(int v)
{
    x -= v;
    x -= v;
    return *this;
}

int Manhattan(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return abs(lhs.x - rhs.x) + abs(rhs.y - lhs.y);
}

float Hypot(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return hypot(Sqr(lhs.x - rhs.x), Sqr(lhs.y - rhs.y));
}

std::ostream& operator<<(std::ostream &out, const SDL_Point &pt)
{
    out << '(' << pt.x << ',' << pt.y << ')';
    return out;
}
