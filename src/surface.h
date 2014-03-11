#ifndef SURFACE_H_
#define SURFACE_H_

#include <functional>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include "SDL.h"

#include "sdl_utils.h"
#include "geometry.h"
#include "macrosota.h"

const int NO_FLAGS = 0;

const int RMASK_DEFAULT = 0;
const int GMASK_DEFAULT = 0;
const int BMASK_DEFAULT = 0;
const int AMASK_DEFAULT = 0;

class Surface;

// RAII for SDL_LockSurface / SDL_UnlockSurface
class SurfaceLocker
{
    const Surface &object;
    bool locked;
public:
    SurfaceLocker(const Surface &surface);
    ~SurfaceLocker();
};

// RAII for SDL_SetColorKey
class ColorKeyLocker
{
    const Surface &object;
    bool oldEnabled;
    Uint32 oldColor;
public:
    ColorKeyLocker(const Surface &surface, bool enabled, Uint32 color);
    ~ColorKeyLocker();
};

// Wrapper around SDL_Surface (with reference counting)
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

// Wrapper for Region-Of-Interest (exactly like OpenCV ROI on Mat class)
// Acts like simple surface, but has very special destructor which
// respect not only the roi-surface, but also referer surface.
// 
// It holds reference to original surface `src' until become out of scoped.
// 
class SurfaceROI : public Surface
{
    SDL_Surface *m_referer;
public:
    SurfaceROI(const Surface &src, const SDL_Rect *roi);
    virtual ~SurfaceROI();
};

void MapSurface(Surface &dst, const std::function<SDL_Color(Uint8, Uint8, Uint8, Uint8)> &);

bool HasPalette(const Surface &surface);

Surface SubSurface(Surface &src, const SDL_Rect *rect);

Surface CopySurface(const Surface &src, const SDL_Rect *srcrect);

Surface CopySurfaceFormat(const Surface &src, int width, int height);

void CopySurfaceColorKey(const Surface &src, Surface &dst);

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect);

void DrawFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color);

void FillFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color);

SDL_Rect SurfaceBounds(const Surface &src);

#endif
