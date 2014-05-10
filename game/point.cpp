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

Point& operator+=(Point &lhs, const SDL_Point &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

Point& operator-=(Point &lhs, const SDL_Point &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

std::ostream& operator<<(std::ostream &out, const SDL_Point &pt)
{
    out << '(' << pt.x << ',' << pt.y << ')';
    return out;
}

int Manhattan(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return abs(lhs.x - rhs.x) + abs(rhs.y - lhs.y);
}

float Hypot(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return hypot(Sqr(lhs.x - rhs.x), Sqr(lhs.y - rhs.y));
}

bool operator==(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

bool operator!=(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

bool operator<(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x < rhs.x) || (lhs.x == rhs.x && lhs.y < rhs.y);
}

bool operator>(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x > rhs.x) || (lhs.x == rhs.x && lhs.y > rhs.y);
}

Point operator-(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}

Point operator+(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point operator*(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x * rhs.x, lhs.y * rhs.y);
}

Point operator/(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x / rhs.x, lhs.y / rhs.y);
}

Point operator-(const SDL_Point &pt, int x)
{
    return Point(pt.x - x, pt.y - x);
}

Point operator+(const SDL_Point &pt, int x)
{
    return Point(pt.x + x, pt.y + x);
}

Point operator*(const SDL_Point &pt, int x)
{
    return Point(pt.x * x, pt.y * x);
}

Point operator/(const SDL_Point &pt, int x)
{
    return Point(pt.x / x, pt.y / x);
}

Point ShiftPoint(const SDL_Point &pt, int x, int y)
{
    return Point(pt.x + x, pt.y + y);
}

Point AlignPoint(const SDL_Rect &rt, double x, double y)
{
    return Point(rt.x + rt.w * (x + 1) / 2,
                 rt.y + rt.h * (y + 1) / 2);
}

bool PointInRect(const SDL_Rect &rect, const SDL_Point &pt)
{
    return PointInRect(rect, pt.x, pt.y);
}

bool PointInRect(const SDL_Rect &rect, int x, int y)
{
    return (rect.x >= x)
        && (rect.y >= y)
        && (rect.w + rect.x > x)
        && (rect.h + rect.y > y);
}

Point RectCenter(const SDL_Rect &rect)
{
    return Point(rect.x + rect.w / 2,
                 rect.y + rect.h / 2);
}

Point TopLeft(const SDL_Rect &rect)
{
    return Point(rect.x, rect.y);
}

Point TopRight(const SDL_Rect &rect)
{
    return Point(rect.x + rect.w, rect.y);
}

Point BottomLeft(const SDL_Rect &rect)
{
    return Point(rect.x, rect.y + rect.h);
}

Point BottomRight(const SDL_Rect &rect)
{
    return Point(rect.x + rect.w, rect.y + rect.y);
}
