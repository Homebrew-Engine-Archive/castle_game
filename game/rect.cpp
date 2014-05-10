#include "rect.h"

#include <iostream>
#include <algorithm>

Rect& Rect::operator=(const SDL_Rect &that)
{
    x = that.x;
    y = that.y;
    w = that.w;
    h = that.h;
    return *this;
}

Rect Normalized(const SDL_Rect &rect)
{
    Rect result = rect;
    
    if(result.w < 0) {
        result.x += result.w;
        result.w = -result.w;
    }

    if(result.h < 0) {
        result.y += result.h;
        result.h = -result.h;
    }

    return result;
}

bool operator==(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return (lhs.x == rhs.x)
        && (lhs.y == rhs.y)
        && (lhs.w == rhs.w)
        && (lhs.h == rhs.h);
}

bool operator!=(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return (lhs.x != rhs.x)
        || (lhs.y != rhs.y)
        || (lhs.w != rhs.w)
        || (lhs.h != rhs.h);
}

Rect UnionRects(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    Rect result;
    SDL_UnionRect(&lhs, &rhs, &result);
    return result;
}

Rect IntersectRects(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    Rect result;
    SDL_IntersectRect(&lhs, &rhs, &result);
    return result;
}

Rect ShiftRect(const SDL_Rect &rect, int xshift, int yshift)
{
    return Rect(rect.x + xshift,
                rect.y + yshift,
                rect.w,
                rect.h);
}

/**
 * \brief Calculate layout rect for src in dst
 *
 * One assumed that src is smaller and dst is larger.
 * The area of dst is mapped onto two-dimensional plane with bounds [-1..1] by x and y.
 *
 * \param src       Inner rect (only width and height matter).
 * \param dst       Outer rect.
 * \param x         Relative x-pos of src center
 * \param y         Relative y-pos of src center
 *
 */
Rect PutIn(const SDL_Rect &src, const SDL_Rect &dst, double x, double y)
{
    // Non-intuitive formulas is for the sake of precision
    const int xcenter = 2 * dst.x + dst.w;
    const int ycenter = 2 * dst.y + dst.h;

    const int xspace = std::max(0, dst.w - src.w);
    const int yspace = std::max(0, dst.h - src.h);

    const int xpos = x * xspace;
    const int ypos = y * yspace;

    Rect rect;
    rect.x = (xcenter + xpos - src.w) / 2;               // dst.x + 0.5*(dst.w + x*xspace - src.w)
    rect.y = (ycenter + ypos - src.h) / 2;               // dst.y + 0.5*(dst.h + y*yspace - src.h)
    rect.w = src.w;
    rect.h = src.h;
    return rect;
}

Rect PadOut(const SDL_Rect &src, int pad)
{
    return Rect(src.x - pad,
                src.y - pad,
                src.w + 2 * pad,
                src.h + 2 * pad);
}

Rect PadIn(const SDL_Rect &src, int pad)
{
    Rect result;
    
    if((src.w >= (2 * pad)) && (src.h >= (2 * pad))) {
        result.x = src.x + pad;
        result.y = src.y + pad;
        result.w = src.w - 2 * pad;
        result.h = src.h - 2 * pad;
    }

    return result;
}

std::ostream& operator<<(std::ostream &out, const SDL_Rect &rect)
{
    out << rect.w << 'x' << rect.h;
    if(rect.x >= 0)
        out << '+';
    out << rect.x;
    if(rect.y >= 0)
        out << '+';
    out << rect.y;
    return out;
}
