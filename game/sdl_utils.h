#ifndef SDL_UTILS_H_
#define SDL_UTILS_H_

#include <iosfwd>
#include <memory>

#include <SDL.h>

struct SDLInitializer final
{
    SDLInitializer();
    ~SDLInitializer();
};

template<class T, void (*D)(T*)>
struct SDLDeleter
{
    void operator()(T *t) const {
        D(t);
    }
};

typedef std::unique_ptr<SDL_Renderer, SDLDeleter<SDL_Renderer, SDL_DestroyRenderer>> RendererPtr;

typedef std::unique_ptr<SDL_Texture, SDLDeleter<SDL_Texture, SDL_DestroyTexture>> TexturePtr;

struct RWCloseDeleter
{
    void operator()(SDL_RWops *src) const {
        SDL_RWclose(src);
    }
};

typedef std::unique_ptr<SDL_RWops, RWCloseDeleter> RWPtr;

typedef std::unique_ptr<SDL_Window, SDLDeleter<SDL_Window, SDL_DestroyWindow>> WindowPtr;

typedef std::unique_ptr<SDL_PixelFormat, SDLDeleter<SDL_PixelFormat, SDL_FreeFormat>> PixelFormatPtr;

typedef std::unique_ptr<SDL_Palette, SDLDeleter<SDL_Palette, SDL_FreePalette>> PalettePtr;

SDL_Rect MakeRect(int x, int y, int w, int h);
SDL_Rect MakeRect(int w, int h);
SDL_Rect MakeRect(const SDL_Point &p1, const SDL_Point &p2);
SDL_Rect MakeRect(const SDL_Point &p);
SDL_Rect MakeRect(const SDL_Point &p, int w, int h);
SDL_Rect MakeEmptyRect();
SDL_Color MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
SDL_Point MakePoint(int x, int y);

SDL_Color GetPixelColor(uint32_t pixel, int format);
uint32_t GetPackedPixel(const char *data, int bytesPerPixel);
void SetPackedPixel(char *data, uint32_t pixel, int bytesPerPixel);

SDL_Rect PutIn(const SDL_Rect &src, const SDL_Rect &dst, double x, double y);
SDL_Rect PadIn(const SDL_Rect &src, int pad);
SDL_Rect ShiftRect(const SDL_Rect &src, int xshift, int yshift);
bool IsInRect(const SDL_Rect &rect, int x, int y);
SDL_Point TopRight(const SDL_Rect &src);
SDL_Point BottomLeft(const SDL_Rect &src);
SDL_Point TopLeft(const SDL_Rect &src);
SDL_Point BottomRight(const SDL_Rect &src);
SDL_Point AlignPoint(const SDL_Rect &dst, double x, double y);
SDL_Point ShiftPoint(const SDL_Point &point, int x, int y);

bool operator==(const SDL_Color &left, const SDL_Color &right);
bool operator!=(const SDL_Color &left, const SDL_Color &right);

std::ostream& operator<<(std::ostream &out, const SDL_Color &color);
std::ostream& operator<<(std::ostream &out, const SDL_Rect &rect);
std::ostream& operator<<(std::ostream &out, const SDL_Point &pt);
std::istream& operator>>(std::istream &in, SDL_Color &color);

#endif
