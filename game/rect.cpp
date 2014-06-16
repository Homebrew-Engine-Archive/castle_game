#include "rect.h"

#include <iostream>
#include <algorithm>

namespace core
{
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
        return Rect(std::min(rect.x, rect.x + rect.w),
                    std::min(rect.y, rect.y + rect.h),
                    std::max(rect.x, rect.x + rect.w),
                    std::max(rect.y, rect.y + rect.h));
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
        const int xcenter = 2 * dst.x + dst.w;
        const int ycenter = 2 * dst.y + dst.h;

        const int xspace = std::max(0, dst.w - src.w);
        const int yspace = std::max(0, dst.h - src.h);

        const int xpos = x * xspace;
        const int ypos = y * yspace;

        Rect rect;
        rect.x = (xcenter + xpos - src.w) / 2;
        rect.y = (ycenter + ypos - src.h) / 2;
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
        Rect result = src;
    
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
}
