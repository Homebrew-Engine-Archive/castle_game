#ifndef SDL_UTILS_H_
#define SDL_UTILS_H_

#include <stdexcept>
#include <memory>
#include <iostream>
#include "SDL.h"

template<class T, class D>
static void ThrowSDLError(const std::unique_ptr<T, D> &ptr)
{
    if(!ptr)
        throw std::runtime_error(SDL_GetError());
}

void ThrowSDLError(const SDL_Surface *surface);
void ThrowSDLError(int code);

struct FreeSurfaceDeleter
{
    void operator()(SDL_Surface *surface) {
        if(surface != NULL)
            SDL_FreeSurface(surface);
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

inline static std::ostream &operator<<(std::ostream &out, const SDL_Rect &rect)
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

inline static std::ostream &operator<<(std::ostream &out, const SDL_Point &pt)
{
    out << '(' << pt.x << ", " << pt.y << ')';
    return out;
}

#endif
