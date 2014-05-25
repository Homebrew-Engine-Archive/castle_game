#include "point.h"

#include <cassert>
#include <cmath>
#include <iostream>

#include <game/rect.h>

namespace
{
    void test()
    {
        constexpr Rect r(100, 100, 200, 200);

        constexpr Point a(0, 0);
        constexpr Point b(100, 100);
        constexpr Point c(99, 150);
        constexpr Point d(150, 99);
        
        static_assert(!PointInRect(r, a), "A in R");
        static_assert(PointInRect(r, b), "B not in R");
        static_assert(!PointInRect(r, c), "C in R");
        static_assert(!PointInRect(r, d), "D in R");
    }
    
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

Point& Point::operator*=(const SDL_Point &that)
{
    x *= that.x;
    y *= that.y;
    return *this;
}

Point& Point::operator/=(const SDL_Point &that)
{
    x /= that.x;
    y /= that.y;
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

Point& Point::operator*=(int v)
{
    x *= v;
    x *= v;
    return *this;
}

Point& Point::operator/=(int v)
{
    x /= v;
    x /= v;
    return *this;
}

int Manhattan(const SDL_Point &L, const SDL_Point &R)
{
    return abs(L.x - R.x) + abs(R.y - L.y);
}

float Hypot(const SDL_Point &L, const SDL_Point &R)
{
    return hypot(L.x - R.x, L.y - R.y);
}

std::ostream& operator<<(std::ostream &out, const SDL_Point &pt)
{
    out << '(' << pt.x << ',' << pt.y << ')';
    return out;
}
