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

Surface::Surface()
    : surface(NULL)
{
}

Surface::Surface(const Surface &that)
    : surface(that)
{
    IncRefCount(surface);
}

Surface::Surface(SDL_Surface *s)
{
    IncRefCount(surface);
    Assign(s);
}

Surface &Surface::operator=(const Surface &that)
{
    SDL_Surface *s = that.Get();
    IncRefCount(s);
    Assign(s);
    return *this;
}

bool Surface::operator==(const Surface &that)
{
    return that.Get() == surface;
}

Surface::~Surface()
{
    Assign(NULL);
}

void Surface::Assign(SDL_Surface *s)
{
    if(Unique(surface))
        SDL_FreeSurface(surface);
    else
        DecRefCount(surface);
    surface = s;
}

void Surface::IncRefCount(SDL_Surface *s)
{
    if(s != NULL)
        ++s->refcount;
}

void Surface::DecRefCount(SDL_Surface *s)
{
    if(s != NULL)
        --s->refcount;
}

bool Surface::Unique(SDL_Surface *s) const
{
    return s && s->refcount == 1;
}

bool Surface::Null() const
{
    return surface == NULL;
}

SDL_Surface *Surface::Get() const
{
    return surface;
}

SDL_Surface *Surface::operator->() const
{
    return surface;
}

Surface::operator SDL_Surface *() const
{
    return surface;
}

void BlitSurface(const Surface &src, const SDL_Rect *srcrect,
                 Surface &dst, SDL_Rect *dstrect)
{
    int srclocked = SDL_MUSTLOCK(src);
    int dstlocked = SDL_MUSTLOCK(dst);
    
    if(srclocked)
        SDL_LockSurface(src);
    if(dstlocked)
        SDL_LockSurface(dst);

    if(SDL_BlitSurface(src, srcrect, dst, dstrect)) {
        throw SDLError(SDL_GetError());
    }
    
    if(srclocked)
        SDL_UnlockSurface(src);
    if(dstlocked)
        SDL_UnlockSurface(dst);
}

void FillRect(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    SDL_FillRect(dst, dstrect, color);
}

void SetColorKey(Surface &dst, Uint32 color, bool enabled)
{
    SDL_SetColorKey(dst, enabled, color);
}

SDL_Rect MakeRect(int x, int y, int w, int h)
{
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    return r;
}

SDL_Rect MakeRect(int w, int h)
{
    SDL_Rect r;
    r.x = 0;
    r.h = 0;
    r.w = w;
    r.h = h;
    return r;
}

SDL_Rect MakeEmptyRect()
{
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = 0;
    r.h = 0;
    return r;
}
