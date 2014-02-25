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
        throw std::runtime_error(SDL_GetError());

}

Surface::Surface()
    : surface(NULL)
{
}

Surface::Surface(const Surface &that)
    : surface(that)
{
    if(surface != NULL)
        IncRefCount(surface);
}

Surface::Surface(SDL_Surface *s)
{
    if(surface != NULL)
        IncRefCount(surface);
    Assign(s);
}

Surface &Surface::operator=(const Surface &that)
{
    SDL_Surface *s = that.Get();
    if(s != NULL)
        IncRefCount(s);
    Assign(s);
    return *this;
}

Surface &Surface::operator=(SDL_Surface *s)
{
    if(s != NULL)
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
    if(surface != NULL) {
        if(Unique(surface))
            SDL_FreeSurface(surface);
        else
            DecRefCount(surface);
    }
    surface = s;
}

void Surface::IncRefCount(SDL_Surface *s)
{
    ++s->refcount;
}

void Surface::DecRefCount(SDL_Surface *s)
{
    --s->refcount;
}

bool Surface::Unique(SDL_Surface *s) const
{
    return s->refcount == 1;
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

SurfaceLocker::SurfaceLocker(Surface &surface)
    : object(surface)
    , locked(0)
{
    if(!object.Null()) {
        locked = SDL_MUSTLOCK(object);
        if(locked) {
            if(SDL_LockSurface(object)) {
                throw std::runtime_error(SDL_GetError());
            }
        }
    }
}

SurfaceLocker::~SurfaceLocker()
{
    if(locked && !object.Null()) {
        SDL_UnlockSurface(object);
    }
}

Surface CopySurface(const Surface &src, const SDL_Rect *srcrect)
{
    Uint32 width = (srcrect == NULL
                ? src->w
                : srcrect->w);
    Uint32 height = (srcrect == NULL
                     ? src->h
                     : srcrect->h);
    Uint32 depth = src->format->BitsPerPixel;
    Uint32 rmask = src->format->Rmask;
    Uint32 gmask = src->format->Gmask;
    Uint32 bmask = src->format->Bmask;
    Uint32 amask = src->format->Amask;
    
    Surface dst(
        width, height, depth,
        rmask, gmask, bmask, amask);

    Uint32 key;
    if(0 == SDL_GetColorKey(src, &key)) {
        SDL_SetColorKey(dst, SDL_TRUE, key);
        SDL_FillRect(dst, NULL, key);
    }
    
    BlitSurface(src, srcrect, dst, NULL);
    
    return dst;
}

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect)
{
    int srclocked = SDL_MUSTLOCK(src);
    int dstlocked = SDL_MUSTLOCK(dst);
    
    if(srclocked)
        SDL_LockSurface(src);
    if(dstlocked)
        SDL_LockSurface(dst);

    if(SDL_BlitSurface(src, srcrect, dst, dstrect)) {
        throw std::runtime_error(SDL_GetError());
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

bool RectIsEmpty(const SDL_Rect &rect)
{
    return rect.w == 0 || rect.h == 0;
}
