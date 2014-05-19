#ifndef POINT_H_
#define POINT_H_

#include <iosfwd>
#include <SDL.h>

class Point : public SDL_Point
{
public:
    constexpr Point()
        : Point(0, 0) {}
    
    constexpr Point(int x, int y)
        : SDL_Point {x, y} {}

    constexpr Point(const Point &that) = default;

    constexpr Point(const SDL_Point &that)
        : Point(that.x, that.y) {}
    
    Point& operator=(const SDL_Point &that);
    Point& operator+=(int v);
    Point& operator-=(int v);
    Point& operator+=(const SDL_Point &that);
    Point& operator-=(const SDL_Point &that);
};

constexpr Point operator-(const SDL_Point &point)
{
    return Point(-point.x, -point.y);
}

constexpr bool operator==(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

constexpr bool operator!=(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

constexpr bool operator<(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x < rhs.x) || (lhs.x == rhs.x && lhs.y < rhs.y);
}

constexpr bool operator>(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return (lhs.x > rhs.x) || (lhs.x == rhs.x && lhs.y > rhs.y);
}

constexpr Point operator+(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

constexpr Point operator-(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}

constexpr Point operator*(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x * rhs.x, lhs.y * rhs.y);
}

constexpr Point operator/(const SDL_Point &lhs, const SDL_Point &rhs)
{
    return Point(lhs.x / rhs.x, lhs.y / rhs.y);
}

constexpr Point operator+(const SDL_Point &pt, int v)
{
    return Point(pt.x + v, pt.y + v);
}

constexpr Point operator-(const SDL_Point &pt, int v)
{
    return Point(pt.x - v, pt.y - v);
}

constexpr Point operator*(const SDL_Point &pt, int v)
{
    return Point(pt.x * v, pt.y * v);
}

constexpr Point operator/(const SDL_Point &pt, int v)
{
    return Point(pt.x / v, pt.y / v);
}

constexpr Point TopLeft(const SDL_Rect &rect)
{
    return Point(rect.x, rect.y);
}

constexpr Point TopRight(const SDL_Rect &rect)
{
    return Point(rect.x + rect.w, rect.y);
}

constexpr Point BottomLeft(const SDL_Rect &rect)
{
    return Point(rect.x, rect.y + rect.h);
}

constexpr Point BottomRight(const SDL_Rect &rect)
{
    return Point(rect.x + rect.w, rect.y + rect.h);
}

constexpr Point RectCenter(const SDL_Rect &rect)
{
    return Point(rect.x + rect.w / 2,
                 rect.y + rect.h / 2);
}

constexpr Point AlignPoint(const SDL_Rect &rt, double x, double y)
{
    return Point(rt.x + rt.w * (x + 1) / 2,
                 rt.y + rt.h * (y + 1) / 2);
}

constexpr bool PointInRect(const SDL_Rect &rect, const SDL_Point &pt)
{
    return (rect.x >= pt.x)
        && (rect.y >= pt.y)
        && (rect.w + rect.x > pt.x)
        && (rect.h + rect.y > pt.y);
}

float Hypot(const SDL_Point &lhs, const SDL_Point &rhs);
int Manhattan(const SDL_Point &lhs, const SDL_Point &rhs);

std::ostream& operator<<(std::ostream &out, const SDL_Point &pt);

#endif
