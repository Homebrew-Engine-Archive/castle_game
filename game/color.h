#ifndef COLOR_H_
#define COLOR_H_

#include <iosfwd>
#include <SDL.h>

class Color : public SDL_Color
{
public:
    constexpr Color()
        : Color(0, 0, 0, 0) {}
    
    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : Color(r, g, b, 255) {}
    
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : SDL_Color {r, g, b, a} {}

    constexpr Color(const Color &that) = default;

    constexpr Color(const SDL_Color &that)
        : Color(that.r, that.g, that.b, that.a) {}
    
    Color(uint32_t argb32, int format);
    Color(uint32_t argb32, const SDL_PixelFormat *format);

    constexpr Color Opaque(int alpha) const {
        return Color(r, g, b, alpha);
    }

    static Color Black() { return Color(0, 0, 0); }
    static Color Red() { return Color(255, 0, 0); }
    static Color Green() { return Color(0, 255, 0); }
    static Color Blue() { return Color(0, 0, 255); }
    static Color White() { return Color(255, 255, 255); }
    static Color Magenta() { return Color(255, 0, 255); }
    static Color Yellow() { return Color(255, 255, 0); }
    static Color Cyan() { return Color(0, 255, 255); }
    static Color Gray() { return Color(128, 128, 128); }
};
    
constexpr Color Inverted(const SDL_Color &color)
{
    return Color(255 - color.r,
                 255 - color.g,
                 255 - color.b,
                 color.a);
}

constexpr bool operator==(const SDL_Color &lhs, const SDL_Color &rhs)
{
    return (lhs.r == rhs.r)
        && (lhs.g == rhs.g)
        && (lhs.b == rhs.b)
        && (lhs.a == rhs.a);
}

constexpr bool operator!=(const SDL_Color &lhs, const SDL_Color &rhs)
{
    return (lhs.r != rhs.r)
        || (lhs.g != rhs.g)
        || (lhs.b != rhs.b)
        || (lhs.a != rhs.a);
}

Color GetPixelColor(uint32_t pixel, int format);
uint32_t GetPackedPixel(const char *data, int bytesPerPixel);
void SetPackedPixel(char *data, uint32_t pixel, int bytesPerPixel);

std::ostream& operator<<(std::ostream &out, SDL_Color const&);
std::istream& operator>>(std::istream &in, SDL_Color&);

#endif // COLOR_H_
