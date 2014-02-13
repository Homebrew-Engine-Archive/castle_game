#include "SDLSurface.h"

SDLSurface::SDLSurface(
    Uint32 width,
    Uint32 height,
    Uint32 depth,
    Uint32 rmask,
    Uint32 gmask,
    Uint32 bmask,
    Uint32 amask)
{
    surface = SDL_CreateRGBSurface(
        0, width, height, depth,
        rmask, gmask, bmask, amask);
    if(surface == NULL)
        throw SDLError(SDL_GetError());
}

SDLSurface::~SDLSurface()
{
    SDL_FreeSurface(surface);
}

SDL_Surface* SDLSurface::GetSurface()
{
    return surface;
}

const SDL_Surface* SDLSurface::GetSurface() const
{
    return surface;
}

SDL_Rect SDLSurface::GetRect() const
{
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = surface->w;
    rect.h = surface->h;
    return rect;
}

void SDLSurface::Blit(SDLSurface &src, SDL_Rect *dstrect, const SDL_Rect *srcrect)
{
    bool locked = SDL_MUSTLOCK(surface);
    if(locked)
        SDL_LockSurface(surface);

    if(0 != SDL_BlitSurface(src.GetSurface(), srcrect, surface, dstrect)) {
        throw SDLError(SDL_GetError());
    }
    if(locked)
        SDL_UnlockSurface(surface);
}

void SDLSurface::SetColorKey(Uint32 color, bool enable)
{
    SDL_SetColorKey(surface, enable, color);
}

int SDLSurface::Width() const
{
    return surface->w;
}

int SDLSurface::Height() const
{
    return surface->h;
}

void* SDLSurface::Bits()
{
    return surface->pixels;
}

const void* SDLSurface::Bits() const
{
    return surface->pixels;
}
