#ifndef RECT_H_
#define RECT_H_

#include <iosfwd>
#include <SDL.h>

#include <game/minmax.h>
#include <game/range.h>

class Rect final : public SDL_Rect
{
public:
    constexpr Rect();
    constexpr Rect(int w, int h);
    constexpr Rect(int x, int y, int w, int h);
    constexpr Rect(const SDL_Point &lt, int w, int h);
    explicit constexpr Rect(const SDL_Point &wh);    
    constexpr Rect(const SDL_Point &lt, const SDL_Point &br);
    constexpr Rect(const SDL_Rect &other);
    explicit constexpr Rect(const SDL_Surface *surface);
    
    constexpr Rect(const Rect &that) = default;
    constexpr Rect(Rect&&) = default;

    Rect& operator=(const Rect &that) = default;
    Rect& operator=(Rect&&) = default;
    
    Rect& operator=(const SDL_Rect &that);
};

constexpr Rect::Rect(int x, int y, int w, int h)
    : SDL_Rect {x, y, w, h}
{
}

constexpr Rect::Rect(int w, int h)
    : Rect(0, 0, w, h)
{
}

constexpr Rect::Rect()
    : Rect(0, 0, 0, 0)
{
}

constexpr Rect::Rect(const SDL_Surface *surface)
    : Rect(surface->w, surface->h)
{
}

constexpr Rect::Rect(const SDL_Point &lt, int w, int h)
    : Rect(lt.x, lt.y, w, h)
{
}

constexpr Rect::Rect(const SDL_Point &lt, const SDL_Point &br)
    : Rect(lt.x, lt.y, br.x - lt.x, br.y - lt.y)
{}

constexpr Rect::Rect(const SDL_Rect &other)
    : Rect(other.x, other.y, other.w, other.h)
{}

constexpr Rect::Rect(const SDL_Point &wh)
    : Rect(wh.x, wh.y)
{
}

constexpr bool operator==(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return (lhs.x == rhs.x)
        && (lhs.y == rhs.y)
        && (lhs.w == rhs.w)
        && (lhs.h == rhs.h);
}

constexpr bool operator!=(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return (lhs.x != rhs.x)
        || (lhs.y != rhs.y)
        || (lhs.w != rhs.w)
        || (lhs.h != rhs.h);
}

constexpr bool Intersects(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return (lhs.x <= rhs.x + rhs.w)
        && (rhs.x <= lhs.x + lhs.w)
        && (lhs.y < rhs.y + rhs.h)
        && (rhs.y < lhs.y + lhs.h);
}

constexpr Rect Translated(const SDL_Rect &rect, const SDL_Point &point)
{
    return Rect(rect.x + point.x, rect.y + point.y, rect.w, rect.h);
}

constexpr bool RectEmpty(const SDL_Rect &rect)
{
    return ((rect.w <= 0) || (rect.h <= 0));
}

constexpr Rect ChopBottom(const SDL_Rect &rect, int howmany)
{
    return Rect(rect.x,
                rect.y,
                rect.w,
                Max(0, rect.h - howmany));
}

constexpr Rect ChopTop(const SDL_Rect &rect, int howmany)
{
    return Rect(rect.x,
                rect.y + Min(howmany, rect.h),
                rect.w,
                Max(0, rect.h - howmany));
}

constexpr Rect ChopLeft(const SDL_Rect &rect, int howmany)
{
    return Rect(rect.x + Min(howmany, rect.w),
                rect.y,
                Max(0, rect.w),
                rect.h);
}

constexpr Rect ChopRight(const SDL_Rect &rect, int howmany)
{
    return Rect(rect.x,
                rect.y,
                Max(0, rect.w),
                rect.h);
}

template<class T>
constexpr const Rect CombineRect(const Range<T> &axisX, const Range<T> &axisY)
{
    return Rect(axisX.min, axisY.min,
                axisX.max - axisX.min,
                axisY.max - axisY.min);
}

constexpr const Range<int> AxisX(const SDL_Rect &rect)
{
    return Range<int>(rect.x, rect.x + rect.w);
}

constexpr const Range<int> AxisY(const SDL_Rect &rect)
{
    return Range<int>(rect.y, rect.y + rect.h);
}

constexpr const Rect Union(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return CombineRect(
        Union(AxisX(lhs), AxisX(rhs)),
        Union(AxisY(lhs), AxisY(rhs)));
}

constexpr const Rect Intersection(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return CombineRect(
        Intersection(AxisX(lhs), AxisX(rhs)),
        Intersection(AxisY(lhs), AxisY(rhs)));
}

std::ostream& operator<<(std::ostream &out, const SDL_Rect &rect);

Rect PutIn(const SDL_Rect &src, const SDL_Rect &dst, double x, double y);
Rect PadIn(const SDL_Rect &src, int pad);
Rect PadOut(const SDL_Rect &src, int pad);
Rect Normalized(const SDL_Rect &src);

#endif // RECT_H_
