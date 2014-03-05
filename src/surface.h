#ifndef SURFACE_H_
#define SURFACE_H_

#include <iostream>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <SDL2/SDL.h>

#include "macrosota.h"

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

struct FreeSurfaceDeleter
{
    void operator()(SDL_Surface *surface) {
        if(surface != NULL)
            SDL_FreeSurface(surface);
    }
};

class Surface
{
protected:
    SDL_Surface *m_surface;
    SDL_Surface *m_referer;
    
    void AddRef(SDL_Surface *s);
    virtual void Assign(SDL_Surface *);    
    
public:
    virtual ~Surface();
    Surface();
    Surface(SDL_Surface *s);
    Surface(const Surface &that);    
    virtual bool Null() const;
    virtual operator SDL_Surface *() const;
    virtual Surface &operator=(SDL_Surface *s);
    virtual Surface &operator=(const Surface &that);
    virtual bool operator==(const Surface &that);
    virtual SDL_Surface *operator->() const;
    virtual void reset();
};

class SurfaceROI : public Surface
{
    SDL_Surface *m_referer;
public:
    SurfaceROI(const Surface &src, const SDL_Rect *roi);
    virtual ~SurfaceROI();
};

bool HasPalette(Surface surface);

Surface SubSurface(Surface &src, const SDL_Rect *rect);

Surface CopySurface(const Surface &src, const SDL_Rect *srcrect);

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect);

void DrawFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color);

void FillFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color);

SDL_Rect SurfaceBounds(const Surface &src);

// Synopsis:
// Put `src' in `dst' relatively offseted by x and y from top left corner
SDL_Rect AlignRect(const SDL_Rect &src, const SDL_Rect &dst, double x, double y);

SDL_Rect MakeRect(int x, int y, int w, int h);

SDL_Rect MakeRect(int w, int h);

SDL_Rect MakeEmptyRect();

SDL_Rect PadIn(const SDL_Rect &src, int pad);

bool IsInRect(const SDL_Rect &rect, int x, int y);

NAMESPACE_BEGIN(std)

std::ostream &operator<<(std::ostream &out, const SDL_Rect &rect);

NAMESPACE_END(std)

#endif
