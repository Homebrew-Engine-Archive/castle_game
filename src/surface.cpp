#include "surface.h"

Surface::Surface(
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

Surface::~Surface()
{
    SDL_FreeSurface(surface);
}

SDL_Surface* Surface::GetSurface()
{
    return surface;
}

const SDL_Surface* Surface::GetSurface() const
{
    return surface;
}

void Surface::Fill(Uint32 color)
{
    Uint32 width = surface->w;
    Uint32 height = surface->h;
    switch(surface->format->BytesPerPixel) {
    case 1:
        {
            Uint8 *bits = reinterpret_cast<Uint8*>(surface->pixels);
            std::fill(bits, bits + (width*height), color);
            break;
        }
    case 2:
        {
            Uint16 *bits = reinterpret_cast<Uint16*>(surface->pixels);
            std::fill(bits, bits + (width*height), color);
            break;
        }
    case 4:
        {
            Uint32 *bits = reinterpret_cast<Uint32*>(surface->pixels);
            std::fill(bits, bits + (width*height), color);
        }
    default:
        SDL_Log("invalid format");
        break;
    }
}

void Surface::Blit(Surface &src, SDL_Rect *dstrect, const SDL_Rect *srcrect)
{
    int locked = SDL_MUSTLOCK(surface);
    if(locked)
        SDL_LockSurface(surface);

    if(SDL_BlitSurface(src.GetSurface(), srcrect, surface, dstrect)) {
        throw SDLError(SDL_GetError());
    }
    if(locked)
        SDL_UnlockSurface(surface);
}

void Surface::SetColorKey(Uint32 color, bool enable)
{
    SDL_SetColorKey(surface, enable, color);
}

int Surface::Width() const
{
    return surface->w;
}

int Surface::Height() const
{
    return surface->h;
}

void* Surface::Bits()
{
    return surface->pixels;
}

const void* Surface::Bits() const
{
    return surface->pixels;
}
