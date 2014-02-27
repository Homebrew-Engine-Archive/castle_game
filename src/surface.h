#ifndef SURFACE_H_
#define SURFACE_H_

#include <stdexcept>
#include <algorithm>
#include <SDL2/SDL.h>

const int RMASK_DEFAULT = 0;
const int GMASK_DEFAULT = 0;
const int BMASK_DEFAULT = 0;
const int AMASK_DEFAULT = 0;

class Surface;
class SurfaceLocker
{
    const Surface &object;
    bool locked;
public:
    SurfaceLocker(const Surface &surface);
    ~SurfaceLocker();
};

class ColorKeyLocker
{
    const Surface &object;
    bool oldEnabled;
    Uint32 oldColor;
public:
    ColorKeyLocker(const Surface &surface, bool enabled, Uint32 color);
    ~ColorKeyLocker();
};

class Surface
{
    SDL_Surface *surface;

    bool Unique(SDL_Surface *) const;
    void IncRefCount(SDL_Surface *);
    void DecRefCount(SDL_Surface *);

    void LogAllocation(SDL_Surface *) const;
    void LogDestruction(SDL_Surface *) const;
    
public:
    Surface();
    
    Surface(int width, int height, int depth, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask);

    Surface(SDL_Surface *s);
    Surface(const Surface &that);
    
    ~Surface();

    void Assign(SDL_Surface *);

    bool Null() const;
    
    operator SDL_Surface *() const;
    
    Surface &operator=(const Surface &that);
    Surface &operator=(SDL_Surface *s);

    bool operator==(const Surface &that);

    SDL_Surface *Get() const;
    SDL_Surface *operator->() const;
    
    static Sint64 numAllocated;
    static Sint64 numDestroyed;
    
};

Surface CopySurface(const Surface &src, const SDL_Rect *srcrect);

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect);

void DrawAlphaFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color);

void FillAlphaRect(Surface &dst, const SDL_Rect *dstrect, Uint32 color);

void SetColorKey(Surface &s, Uint32 color, bool enabled = true);

bool RectIsEmpty(const SDL_Rect &rect);

SDL_Rect MakeRect(int x, int y, int w, int h);

SDL_Rect MakeRect(int w, int h);

SDL_Rect MakeEmptyRect();



#endif
