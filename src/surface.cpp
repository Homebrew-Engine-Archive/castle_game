#include "surface.h"

SurfaceLocker::SurfaceLocker(const Surface &surface)
    : object(surface)
    , locked(false)
{
    if(!object.Null()) {
        if(SDL_MUSTLOCK(object)) {
            if(0 == SDL_LockSurface(object))
                locked = true;
        }
    }
}

SurfaceLocker::~SurfaceLocker()
{
    if(locked && !object.Null()) {
        SDL_UnlockSurface(object);
    }
}

ColorKeyLocker::ColorKeyLocker(const Surface &surface, bool enabled, Uint32 color)
    : object(surface)
{
    if(!object.Null()) {
        oldEnabled =
            (0 == SDL_GetColorKey(object, &oldColor));
        SDL_SetColorKey(object, enabled, color);
    }
}

ColorKeyLocker::~ColorKeyLocker()
{
    if(!object.Null()) {
        SDL_SetColorKey(object, oldEnabled, oldColor);
    }
}

Surface::Surface(int width, int height, int depth, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask)
{
    surface = SDL_CreateRGBSurface(
        0, width, height, depth,
        rmask, gmask, bmask, amask);
    if(surface != NULL) {
        IncRefCount(surface);
        LogAllocation(surface);
    }
}

Surface::Surface()
    : surface(NULL)
{
}

Surface::~Surface()
{
    Assign(NULL);
}

Surface::Surface(const Surface &that)
    : surface(that.Get())
{
    if(surface != NULL)
        IncRefCount(surface);
}

Surface::Surface(SDL_Surface *s)
    : surface(s)
{
    if(surface != NULL) {
        LogAllocation(surface);
        IncRefCount(surface);
    }
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

void Surface::Assign(SDL_Surface *s)
{
    if(surface != NULL) {
        if(Unique(surface)) {
            LogDestruction(surface);
            SDL_FreeSurface(surface);
        } else {
            DecRefCount(surface);
        }
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
    // One reference for `s' surface itself (it is just allocated that way)
    // And the other is for our surface member
    return s->refcount <= 2;
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

void Surface::LogAllocation(SDL_Surface *s) const
{
    numAllocated += 1;
    //SDL_Log("Surface allocated: %08X", s);
}

void Surface::LogDestruction(SDL_Surface *s) const
{
    numDestroyed += 1;
    //SDL_Log("Surface freed: %08X", s);
}

Surface::operator SDL_Surface *() const
{
    return surface;
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
    
    Surface dst = SDL_CreateRGBSurface(
        0, width, height, depth,
        rmask, gmask, bmask, amask);
    
    Uint32 key;
    bool enabled = 
        (0 == SDL_GetColorKey(src, &key));
    
    ColorKeyLocker lock(src, SDL_FALSE, key);
    BlitSurface(src, srcrect, dst, NULL);
    SDL_SetColorKey(dst, enabled, key);
    
    return dst;
}

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect)
{
    SurfaceLocker srcLock(src);
    SurfaceLocker dstLock(dst);

    if(SDL_BlitSurface(src, srcrect, dst, dstrect)) {
        throw std::runtime_error(SDL_GetError());
    }
}

void DrawAlphaFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    SDL_Log("DrawAlphaFrame: not implemented");
}

void FillAlphaRect(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    SDL_Log("FillAlphaRect: not implemented");
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

Sint64 Surface::numAllocated = 0;
Sint64 Surface::numDestroyed = 0;
