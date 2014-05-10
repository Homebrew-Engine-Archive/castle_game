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
    
    Color(uint32_t argb32, int format);
    Color(uint32_t argb32, const SDL_PixelFormat *format);
};

Color GetPixelColor(uint32_t pixel, int format);
uint32_t GetPackedPixel(const char *data, int bytesPerPixel);
void SetPackedPixel(char *data, uint32_t pixel, int bytesPerPixel);

Color Inverted(const SDL_Color &color);

bool operator==(SDL_Color const&, SDL_Color const&);
bool operator!=(SDL_Color const&, SDL_Color const&);

std::ostream& operator<<(std::ostream &out, SDL_Color const&);
std::istream& operator>>(std::istream &in, SDL_Color&);

#endif // COLOR_H_
