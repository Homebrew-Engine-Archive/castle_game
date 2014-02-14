#ifndef SDLSURFACE_H_
#define SDLSURFACE_H_

#include <algorithm>

#include <SDL2/SDL.h>

#include "errors.h"
#include "SDLRect.h"

const int RMASK_DEFAULT = 0;
const int GMASK_DEFAULT = 0;
const int BMASK_DEFAULT = 0;
const int AMASK_DEFAULT = 0;

class SDLSurface
{
    SDL_Surface *surface;
public:
    SDLSurface(
        Uint32 width,
        Uint32 height,
        Uint32 depth,
        Uint32 rmask,
        Uint32 gmask,
        Uint32 bmask,
        Uint32 amask);
    
    ~SDLSurface();

    void Blit(
        SDLSurface &src,
        SDL_Rect *dstrect,
        const SDL_Rect *srcrect = NULL);

    void SetColorKey(Uint32 color, bool enable = true);
    
    SDL_Surface* GetSurface();
    const SDL_Surface* GetSurface() const;
    SDL_Rect GetRect() const;

    void Fill(Uint32 color);

    void* Bits();
    const void* Bits() const;

    int Width() const;
    int Height() const;
    
};

#endif
