#include "sdl_utils.h"

#include <game/endianness.h>

#include <stdexcept>
#include <iostream>
#include <initializer_list>

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
