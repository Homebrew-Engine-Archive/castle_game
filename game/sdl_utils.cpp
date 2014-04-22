#include "sdl_utils.h"

#include <game/endianness.h>

#include <stdexcept>
#include <iostream>
#include <initializer_list>

SDLInitializer::SDLInitializer()
{
    SDL_SetMainReady();
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

SDLInitializer::~SDLInitializer()
{
    SDL_Quit();
}

SDL_Color MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return SDL_Color { r, g, b, a };
}

SDL_Color GetPixelColor(uint32_t pixel, int format)
{
    SDL_Color rgba;

    PixelFormatPtr pf(SDL_AllocFormat(format));
    SDL_GetRGBA(pixel, pf.get(), &rgba.r, &rgba.g, &rgba.b, &rgba.a);
    
    return rgba;
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

std::ostream &operator<<(std::ostream &out, const SDL_Color &color)
{
    out << '(';
    out << static_cast<int>(color.r) << 'r'
        << static_cast<int>(color.g) << 'g'
        << static_cast<int>(color.b) << 'b'
        << static_cast<int>(color.a) << 'a';
    out << ')';
    return out;
}

std::istream& operator>>(std::istream &in, SDL_Color &color)
{
    if(in.get() != '#') {
        in.unget();
    }

    uint32_t value = 0;
    in >> std::hex >> value;

    /** 0x11223344 is has alpha - 11, red -  22, green - 33, blue - 44 **/
    value = Endian::SwapLittle<uint32_t>(value);

    color.a = (value & 0xff000000) >> 24;
    color.r = (value & 0x00ff0000) >> 16;
    color.g = (value & 0x0000ff00) >> 8;
    color.b = (value & 0x000000ff) >> 0;
    
    return in;
}

bool operator==(const SDL_Color &left, const SDL_Color &right)
{
    return (left.r == right.r)
        && (left.g == right.g)
        && (left.b == right.b)
        && (left.a == right.a);
}

bool operator!=(const SDL_Color &left, const SDL_Color &right)
{
    return !(left == right);
}

uint32_t GetPackedPixel(const char *data, int bytesPerPixel)
{
    switch(bytesPerPixel) {
    case 1:
        return *reinterpret_cast<uint8_t const*>(data);
    case 2:
        return *reinterpret_cast<uint16_t const*>(data);
    case 3:
        return Endian::SwapBig<uint32_t>(data[2] + (data[1] << 8) + (data[0] << 16));
    case 4:
        return *reinterpret_cast<uint32_t const*>(data);
    default:
        throw std::runtime_error("Unknown bpp");
    }
}

// NOTE I ignore endianness
void SetPackedPixel(char *data, uint32_t pixel, int bytesPerPixel)
{
    switch(bytesPerPixel) {
    case 1:
        data[0] = pixel & 0xff;
        return;
        
    case 2:
        data[0] = pixel & 0xff;
        data[1] = pixel & 0xff00;
        return;

    case 3:
        data[0] = pixel & 0xff;
        data[1] = pixel & 0xff00;
        data[2] = pixel & 0xff0000;
        return;

    case 4:
        data[0] = pixel & 0xff;
        data[1] = pixel & 0xff00;
        data[2] = pixel & 0xff0000;
        data[3] = pixel & 0xff000000;
        return;

    default:
        throw std::runtime_error("Unknown bpp");
    }
}


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
