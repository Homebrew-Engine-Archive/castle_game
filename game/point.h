#ifndef POINT_H_
#define POINT_H_

#include <iosfwd>
#include <SDL.h>

class Point final : public SDL_Point
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
    Point& operator*=(int v);
    Point& operator/=(int v);
    Point& operator+=(const SDL_Point &that);
    Point& operator-=(const SDL_Point &that);
    Point& operator*=(const SDL_Point &that);
    Point& operator/=(const SDL_Point &that);
};

constexpr Point operator-(const SDL_Point &point)
{
    return Point(-point.x, -point.y);
}

constexpr bool operator==(const SDL_Point &L, const SDL_Point &R)
{
    return (L.x == R.x) && (L.y == R.y);
}

constexpr bool operator!=(const SDL_Point &L, const SDL_Point &R)
{
    return (L.x != R.x) || (L.y != R.y);
}

constexpr bool operator<(const SDL_Point &L, const SDL_Point &R)
{
    return (L.x < R.x) || (L.x == R.x && L.y < R.y);
}

constexpr bool operator>(const SDL_Point &L, const SDL_Point &R)
{
    return (L.x > R.x) || (L.x == R.x && L.y > R.y);
}

constexpr Point operator+(const SDL_Point &L, const SDL_Point &R)
{
    return Point(L.x + R.x, L.y + R.y);
}

constexpr Point operator-(const SDL_Point &L, const SDL_Point &R)
{
    return Point(L.x - R.x, L.y - R.y);
}

constexpr Point operator*(const SDL_Point &L, const SDL_Point &R)
{
    return Point(L.x * R.x, L.y * R.y);
}

constexpr Point operator/(const SDL_Point &L, const SDL_Point &R)
{
    return Point(L.x / R.x, L.y / R.y);
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
    return (pt.x >= rect.x)
        && (pt.y >= rect.y)
        && (pt.x <= rect.x + rect.w)
        && (pt.y <= rect.y + rect.h);
}

constexpr Point ClipToRect(const SDL_Rect &rect, const SDL_Point &pt)
{
    return Point((pt.x - rect.x) % rect.w, (pt.y - rect.y) % rect.h);
}

float Hypot(const SDL_Point &L, const SDL_Point &R);
int Manhattan(const SDL_Point &L, const SDL_Point &R);

std::ostream& operator<<(std::ostream &out, const SDL_Point &pt);

#endif
