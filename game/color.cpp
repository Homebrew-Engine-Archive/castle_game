#include "color.h"

#include <stdexcept>
#include <iostream>

#include <game/endianness.h>
#include <game/sdl_utils.h>

namespace core
{
    const Color PixelToColor(uint32_t pixel, uint32_t format)
    {
        const PixelFormatPtr fmt(SDL_AllocFormat(format));
        if(!fmt) {
            throw sdl_error();
        }
        return PixelToColor(pixel, *fmt);
    }

    const Color PixelToColor(uint32_t pixel, const SDL_PixelFormat &format)
    {
        Color temp;
        SDL_GetRGBA(pixel, &format, &temp.r, &temp.g, &temp.b, &temp.a);
        return temp;
    }

    uint32_t ColorToPixel(const Color &color, uint32_t format)
    {
        const PixelFormatPtr fmt(SDL_AllocFormat(format));
        if(!fmt) {
            throw sdl_error();
        }
        return ColorToPixel(color, *fmt);
    }

    uint32_t ColorToPixel(const Color &color, const SDL_PixelFormat &format)
    {
        return SDL_MapRGBA(&format, color.r, color.g, color.b, color.a);
    }
    
    uint32_t GetPackedPixel(const char *data, int bytesPerPixel)
    {
        switch(bytesPerPixel) {
        case 1:
            return *reinterpret_cast<uint8_t const*>(data);
        case 2:
            return *reinterpret_cast<uint16_t const*>(data);
        case 3:
            return core::SwapBig<uint32_t>(data[2] + (data[1] << 8) + (data[0] << 16));
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
    value = core::SwapLittle<uint32_t>(value);

    color.a = (value & 0xff000000) >> 24;
    color.r = (value & 0x00ff0000) >> 16;
    color.g = (value & 0x0000ff00) >> 8;
    color.b = (value & 0x000000ff) >> 0;
    
    return in;
}
