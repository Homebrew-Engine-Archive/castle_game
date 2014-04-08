#include "sdl_utils.h"

#include <iostream>
#include <initializer_list>

SDL_Color MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return SDL_Color { r, g, b, a };
}

std::ostream &operator<<(std::ostream &out, const SDL_Rect &rect)
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

std::ostream &operator<<(std::ostream &out, const SDL_Point &pt)
{
    out << '(' << pt.x << ", " << pt.y << ')';
    return out;
}
