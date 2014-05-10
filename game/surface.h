#ifndef SURFACE_H_
#define SURFACE_H_

#include <SDL.h>

#include <game/rect.h>

class Color;

/**
 * \brief Wrapper for SDL_Surface with reference counting
 */
class Surface
{
protected:
    SDL_Surface *mSurface;
    void Assign(SDL_Surface*);
    
public:
    Surface();
    Surface(SDL_Surface*);
    Surface(Surface const&);
    virtual ~Surface();
    bool Null() const;
    operator SDL_Surface*() const;
    bool operator!() const;
    Surface& operator=(SDL_Surface*);
    Surface& operator=(Surface const&);
    bool operator==(Surface const&);
    SDL_Surface* operator->() const;
    void reset(SDL_Surface *surface = nullptr);
};

/**
 * \brief RAII for SDL_LockSurface and SDL_UnlockSurface functions call
 */
class SurfaceLocker
{
    const Surface &mObject;
    bool mLocked;
public:
    SurfaceLocker(const Surface &surface);
    // TODO implement copying surface on copying locker
    SurfaceLocker(SurfaceLocker const&) = delete;
    SurfaceLocker &operator=(SurfaceLocker const&) = delete;
    virtual ~SurfaceLocker();
};

/**
 * \brief Region-Of-Interest of surface object
 *
 * It is a surface like object which shares some memory with
 * it's parent surface object. Exactly like OpenCV's Mat class can do .
 *
 * \note Deallocation of parent object isn't invalidate such kind of object.
 *
 * It holds reference to the parent surface so neither this nor parent
 * surface doesn't intent deallocation of each other.
 *
 * \todo This class is not intended to work together with RLE accel. How we can do that?
 *
 */
class SurfaceView : public Surface
{
    Surface mReferer;
public:
    SurfaceView(Surface &src, const Rect &clip);
};

bool HasPalette(const Surface &surface);

void MapSurface(Surface &dst, Color func(uint8_t, uint8_t, uint8_t, uint8_t));

Surface CreateSurface(int width, int height, const SDL_PixelFormat *format);
Surface CreateSurface(int width, int height, int format);

Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat *format);
Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, int format);

void CopyColorKey(SDL_Surface *src, SDL_Surface *dst);

void BlitSurface(const Surface &src, const Rect &srcrect, Surface &dst, const Rect &dstrect);

void DrawFrame(Surface &dst, const Rect &dstrect, const Color &color);

void FillFrame(Surface &dst, const Rect &dstrect, const Color &color);

void BlurSurface(Surface &dst, int radius);

void TransformSurface(const Surface &surface, Color(Color const&));

Rect SurfaceBounds(const Surface &src);

/**
 * \brief Wrapper around reinterpret_cast on surface->pixels
 */
inline char* GetPixels(Surface &surface)
{
    return reinterpret_cast<char *>(surface->pixels);
}

inline char const* ConstGetPixels(const Surface &surface)
{
    return reinterpret_cast<char const*>(surface->pixels);
}

#endif
