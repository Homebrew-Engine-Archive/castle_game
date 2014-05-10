#include "color.h"

#include <stdexcept>
#include <iostream>

#include <game/endianness.h>
#include <game/sdl_utils.h>

Color::Color(uint32_t argb32, int format)
{
    PixelFormatPtr fmt(SDL_AllocFormat(format));
    SDL_GetRGBA(argb32, fmt.get(), &r, &g, &b, &a);
}

Color::Color(uint32_t argb32, const SDL_PixelFormat *format)
{
    SDL_GetRGBA(argb32, format, &r, &g, &b, &a);
}

Color GetPixelColor(uint32_t argb32, int format)
{
    return Color(argb32, format);
}
    
std::ostream& operator<<(std::ostream &out, const SDL_Color &color)
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

    /** 0x11223344 has ALPHA 0x11, RED 0x22, GREEN 0x33, BLUE 0x44 **/
    value = Endian::SwapLittle<uint32_t>(value);

    color.a = (value & 0xff000000) >> 24;
    color.r = (value & 0x00ff0000) >> 16;
    color.g = (value & 0x0000ff00) >> 8;
    color.b = (value & 0x000000ff) >> 0;
    
    return in;
}

bool operator==(const SDL_Color &lhs, const SDL_Color &rhs)
{
    return (lhs.r == rhs.r)
        && (lhs.g == rhs.g)
        && (lhs.b == rhs.b)
        && (lhs.a == rhs.a);
}

bool operator!=(const SDL_Color &lhs, const SDL_Color &rhs)
{
    return (lhs.r != rhs.r)
        || (lhs.g != rhs.g)
        || (lhs.b != rhs.b)
        || (lhs.a != rhs.a);
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

// \note I'm assuming little-endian despite everything else
void SetPackedPixel(char *data, uint32_t pixel, int bytesPerPixel)
{
    switch(bytesPerPixel) {
    case 4:
        data[3] = (pixel & 0xff000000) >> 24;

    case 3:
        data[2] = (pixel & 0x00ff0000) >> 16;

    case 2:
        data[1] = (pixel & 0x0000ff00) >> 8;
        
    case 1:
        data[0] = (pixel & 0x000000ff) >> 0;
        return;

    default:
        throw std::runtime_error("Unknown bpp");
    }
}
    
Color Inverted(const SDL_Color &color)
{
    return Color(255 - color.r,
                 255 - color.g,
                 255 - color.b,
                 color.a);
}
