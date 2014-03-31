#ifndef SDL_UTILS_H_
#define SDL_UTILS_H_

#include <SDL.h>
#include <stdexcept>
#include <memory>
#include <iosfwd>

struct FreeSurfaceDeleter
{
    void operator()(SDL_Surface *surface) {
        if(surface != NULL) {
            SDL_FreeSurface(surface);
        }
    }
};

typedef std::unique_ptr<SDL_Surface, FreeSurfaceDeleter> SurfacePtr;

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

struct FreePaletteDeleter
{
    void operator()(SDL_Palette *palette) const {
        if(palette != NULL)
            SDL_FreePalette(palette);
    }
};

typedef std::unique_ptr<SDL_Palette, FreePaletteDeleter> PalettePtr;

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

SDL_Color MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

std::ostream &operator<<(std::ostream &out, const SDL_Rect &rect);
std::ostream &operator<<(std::ostream &out, const SDL_Point &pt);

void PrintRendererInfo(std::ostream &out, const SDL_RendererInfo &info);

#endif
