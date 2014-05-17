#ifndef RECT_H_
#define RECT_H_

#include <iosfwd>
#include <SDL.h>

class Rect : public SDL_Rect
{
public:
    constexpr Rect()
        : Rect(0, 0, 0, 0) {}
    
    constexpr Rect(int w, int h)
        : Rect(0, 0, w, h) {}
    
    constexpr Rect(int x, int y, int w, int h)
        : SDL_Rect {x, y, w, h} {}
    
    constexpr Rect(const SDL_Point &lt, int w, int h)
        : Rect(lt.x, lt.y, w, h) {}

    constexpr explicit Rect(const SDL_Point &wh)
        : Rect(wh.x, wh.y) {}
    
    constexpr Rect(int x, int y, const SDL_Point &br)
        : Rect(x, y, br.x - x, br.y - y) {}
    
    constexpr Rect(const SDL_Point &lt, const SDL_Point &br)
        : Rect(lt.x, lt.y, br.x - lt.x, br.y - lt.y) {}

    constexpr Rect(const SDL_Rect &other)
        : Rect(other.x, other.y, other.w, other.h) {}

    constexpr Rect(const Rect &that) = default;

    constexpr Rect(const SDL_Surface *surface)
        : Rect(surface->w, surface->h) {}

    Rect& operator=(const SDL_Rect &that);
};

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
        && (lhs.y <= rhs.y + rhs.h)
        && (rhs.y <= lhs.y + lhs.h);
}

Rect PutIn(const SDL_Rect &src, const SDL_Rect &dst, double x, double y);
Rect PadIn(const SDL_Rect &src, int pad);
Rect PadOut(const SDL_Rect &src, int pad);
Rect Normalized(const SDL_Rect &src);

Rect UnionRects(const SDL_Rect &lhs, const SDL_Rect &rhs);
Rect IntersectRects(const SDL_Rect &lhs, const SDL_Rect &rhs);

std::ostream& operator<<(std::ostream &out, const SDL_Rect &rect);

#endif // RECT_H_
