#ifndef SURFACE_H_
#define SURFACE_H_

#include <functional>
#include "SDL.h"
#include "sdl_utils.h"

const int NoFlags = 0;
const int DefaultRedMask = 0;
const int DefaultGreenMask = 0;
const int DefaultBlueMask = 0;
const int DefaultAlphaMask = 0;

/**
 * Wrapper for SDL_Surface's pointer designed with intention
 * to count references into SDL_Surface::refcount.
 */
class Surface
{
protected:
    SDL_Surface *mSurface;
    void Assign(SDL_Surface *);
    
public:
    Surface();
    Surface(SDL_Surface *s);
    Surface(const Surface &that);
    virtual ~Surface();
    bool Null() const;
    operator SDL_Surface *() const;
    Surface &operator=(SDL_Surface *s);
    Surface &operator=(const Surface &that);
    bool operator==(const Surface &that);
    SDL_Surface *operator->() const;
    void reset();
};

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
    uint32_t oldColor;
public:
    ColorKeyLocker(const Surface &surface, bool enabled, uint32_t color);
    ~ColorKeyLocker();
};

// Wrapper for Region-Of-Interest (exactly like OpenCV ROI on Mat class)
// Acts like simple surface, but has very special destructor which
// respect not only the roi-surface, but also referer surface.
// 
// It holds reference to original surface `src' until becomes out-of-scope.
// 
class SurfaceROI : public Surface
{
    SDL_Surface *mReferer;
public:
    SurfaceROI(const Surface &src, const SDL_Rect *roi);
    virtual ~SurfaceROI();
};

typedef std::function<SDL_Color(uint8_t, uint8_t, uint8_t, uint8_t)> PixelMapper;

void MapSurface(Surface &dst, PixelMapper);

bool HasPalette(const Surface &surface);

Surface CopySurfaceFormat(const Surface &src, int width, int height);

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect);

void DrawFrame(Surface &dst, const SDL_Rect *dstrect, uint32_t color);

void FillFrame(Surface &dst, const SDL_Rect *dstrect, uint32_t color);

void BlurSurface(Surface &dst, int radius);

SDL_Rect SurfaceBounds(const Surface &src);

#endif
