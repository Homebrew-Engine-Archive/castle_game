#ifndef SURFACE_H_
#define SURFACE_H_

#include <algorithm>

#include <SDL2/SDL.h>

#include "errors.h"
#include "SDLRect.h"

const int RMASK_DEFAULT = 0;
const int GMASK_DEFAULT = 0;
const int BMASK_DEFAULT = 0;
const int AMASK_DEFAULT = 0;

class Surface
{
    SDL_Surface *surface;

    bool Unique(SDL_Surface *) const;
    void IncRefCount(SDL_Surface *);
    void DecRefCount(SDL_Surface *);
    
public:
    Surface();
    
    Surface(Uint32 width,
            Uint32 height,
            Uint32 depth,
            Uint32 rmask,
            Uint32 gmask,
            Uint32 bmask,
            Uint32 amask);

    Surface(SDL_Surface *s);
    
    Surface(const Surface &that);
    
    ~Surface();

    void Assign(SDL_Surface *);

    bool Null() const;
    
    operator SDL_Surface *() const;
    
    Surface &operator=(const Surface &that);

    SDL_Surface *Get() const;
    SDL_Surface *operator->() const;
    
};

void BlitSurface(const Surface &src, const SDL_Rect *srcrect,
                 Surface &dst, SDL_Rect *dstrect);

void FillRect(Surface &dst, const SDL_Rect *dstrect, Uint32 color);

void SetColorKey(Surface &s, Uint32 color, bool enabled = true);

SDL_Rect MakeRect(int x, int y, int w, int h);

SDL_Rect MakeRect(int w, int h);

SDL_Rect MakeEmptyRect();



#endif
