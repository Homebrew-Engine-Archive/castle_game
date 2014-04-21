#ifndef SDL_UTILS_H_
#define SDL_UTILS_H_

#include <iosfwd>
#include <memory>

#include <SDL.h>

struct DestroyRendererDeleter
{
    void operator()(SDL_Renderer *renderer) const {
        if(renderer != NULL) {
            SDL_DestroyRenderer(renderer);
        }
    }
};


typedef std::unique_ptr<SDL_Renderer, DestroyRendererDeleter> RendererPtr;

struct DestroyTextureDeleter
{
    void operator()(SDL_Texture *texture) const {
        if(texture != NULL) {
            SDL_DestroyTexture(texture);
        }
    }
};

typedef std::unique_ptr<SDL_Texture, DestroyTextureDeleter> TexturePtr;

struct RWCloseDeleter
{
    void operator()(SDL_RWops *src) const {
        if(src != NULL)
            SDL_RWclose(src);
    }
};

typedef std::unique_ptr<SDL_RWops, RWCloseDeleter> RWPtr;

struct DestroyWindowDeleter
{
    void operator()(SDL_Window *window) const {
        if(window != NULL)
            SDL_DestroyWindow(window);
    }
};

typedef std::unique_ptr<SDL_Window, DestroyWindowDeleter> WindowPtr;

struct FreeFormatDeleter
{
    void operator()(SDL_PixelFormat *format) const {
        if(format != NULL) {
            SDL_FreeFormat(format);
        }
    }
};

typedef std::unique_ptr<SDL_PixelFormat, FreeFormatDeleter> PixelFormatPtr;

struct FreePaletteDeleter
{
    void operator()(SDL_Palette *palette) const {
        if(palette != NULL) {
            SDL_FreePalette(palette);
        }
    }
};

typedef std::unique_ptr<SDL_Palette, FreePaletteDeleter> PalettePtr;

SDL_Color MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
SDL_Color GetPixelColor(uint32_t pixel, int format);
uint32_t GetPackedPixel(const char *data, int bytesPerPixel);
void SetPackedPixel(char *data, uint32_t pixel, int bytesPerPixel);

bool operator==(const SDL_Color &left, const SDL_Color &right);
bool operator!=(const SDL_Color &left, const SDL_Color &right);

std::ostream& operator<<(std::ostream &out, const SDL_Color &color);
std::ostream& operator<<(std::ostream &out, const SDL_Rect &rect);
std::ostream& operator<<(std::ostream &out, const SDL_Point &pt);
std::istream& operator>>(std::istream &in, SDL_Color &color);

#endif
