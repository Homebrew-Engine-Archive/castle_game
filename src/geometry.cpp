#include "geometry.h"

SDL_Rect PutIn(const SDL_Rect &src, const SDL_Rect &dst, double x, double y)
{
    // Unintuitive formulas is for the sake of precision
    // int xcenter = dst.x + dst.w / 2;
    // int ycenter = dst.y + dst.h / 2;
    int xcenter = 2 * dst.x + dst.w;
    int ycenter = 2 * dst.y + dst.h;

    int xspace = std::max(0, dst.w - src.w);
    int yspace = std::max(0, dst.h - src.h);

    // int xpos = x * xspace / 2;
    // int ypos = y * yspace / 2;
    int xpos = x * xspace;
    int ypos = y * yspace;

    SDL_Rect rect;
    // rect.x = xcenter + xpos - src.w / 2;
    // rect.y = ycenter + ypos - src.h / 2;
    rect.x = (xcenter + xpos - src.w) / 2;
    rect.y = (ycenter + ypos - src.h) / 2;
    rect.w = src.w;
    rect.h = src.h;
    return rect;
}

SDL_Rect PadIn(const SDL_Rect &src, int pad)
{
    if((src.w >= (2 * pad)) && (src.h >= (2 * pad))) {
        return MakeRect(
            src.x + pad,
            src.y + pad,
            src.w - 2 * pad,
            src.h - 2 * pad);
    }

    return src;
}

SDL_Rect MakeRect(int x, int y, int w, int h)
{
    return SDL_Rect {x, y, w, h};
}

SDL_Rect MakeRect(int w, int h)
{
    return SDL_Rect {0, 0, w, h};
}

SDL_Rect MakeEmptyRect()
{
    return SDL_Rect {0, 0, 0, 0};
}

bool IsInRect(const SDL_Rect &rect, int x, int y)
{
    return ((rect.x >= x) && (x < rect.w + rect.x))
        && ((rect.y >= y) && (y < rect.h + rect.y));
}

SDL_Point MakePoint(int x, int y)
{
    return SDL_Point {x, y};
}

SDL_Point TopLeft(const SDL_Rect &src)
{
    return MakePoint(src.x, src.y);
}

SDL_Point BottomRight(const SDL_Rect &src)
{
    return MakePoint(src.x + src.w,
                     src.y + src.h);
}

SDL_Point BottomLeft(const SDL_Rect &src)
{
    return MakePoint(src.x,
                     src.y + src.h);
}

SDL_Point TopRight(const SDL_Rect &src)
{
    return MakePoint(src.x + src.w,
                     src.y);
}

SDL_Point AlignPoint(const SDL_Rect &rect, double x, double y)
{
    return MakePoint(rect.x + rect.w * (x + 1) / 2,
                     rect.y + rect.h * (y + 1) / 2);
}

SDL_Rect MakeRect(const SDL_Point &p1, const SDL_Point &p2)
{
    return MakeRect(std::min(p1.x, p2.x),
                    std::min(p1.y, p2.y),
                    std::abs(p1.x - p2.x),
                    std::abs(p1.y - p2.y));
}

SDL_Rect MakeRect(const SDL_Point &p)
{
    return MakeRect(p.x, p.y);
}

SDL_Rect ShiftRect(const SDL_Rect &rect, int xshift, int yshift)
{
    return MakeRect(rect.x + xshift,
                    rect.y + yshift,
                    rect.w,
                    rect.h);
}

SDL_Point ShiftPoint(const SDL_Point &point, int x, int y)
{
    return MakePoint(point.x + x, point.y + y);
}
