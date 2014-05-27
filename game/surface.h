#ifndef SURFACE_H_
#define SURFACE_H_

#include <SDL.h>

class Rect;
class Point;
class Color;

/**
 * \brief Wrapper for SDL_Surface with reference counting
 */
class Surface final
{
protected:
    SDL_Surface *mSurface;
    void Assign(SDL_Surface*);
    
public:
    Surface();
    explicit Surface(SDL_Surface*);
    Surface(Surface const&);
    ~Surface();
    bool Null() const;
    operator SDL_Surface*() const;
    bool operator!() const;
    Surface& operator=(SDL_Surface*);
    Surface& operator=(Surface const&);
    SDL_Surface* operator->() const;
    void Reset(SDL_Surface *surface = nullptr);
};

/**
 * \brief RAII for SDL_LockSurface and SDL_UnlockSurface functions call
 */
class SurfaceLocker final
{
    const Surface &mObject;
    bool mLocked;
public:
    explicit SurfaceLocker(const Surface &surface);
    // TODO implement copying surface on copying locker
    SurfaceLocker(SurfaceLocker const&) = delete;
    SurfaceLocker &operator=(SurfaceLocker const&) = delete;
    ~SurfaceLocker();
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
 * \todo This class is not intended to work together with RLE accel. Can we deal with it?
 *
 */
class SurfaceView final
{
    Surface mSurface;
    const Surface &mParentRef;
public:
    SurfaceView(const Surface &src, const Rect &clip);
    SurfaceView(Surface &src, const Rect &clip);

    inline Surface& View() { return mSurface; }
    inline Surface const& View() const { return mSurface; }
};

class SurfaceColorModSetter final
{
    Surface surface;
    uint8_t redMod, greenMod, blueMod;
public:
    SurfaceColorModSetter(const Surface &src, const Color &color);
    ~SurfaceColorModSetter();
};

bool HasPalette(const Surface &surface);

void MapSurface(Surface &dst, Color func(uint8_t, uint8_t, uint8_t, uint8_t));

const Surface CreateSurface(int width, int height, const SDL_PixelFormat &format);
const Surface CreateSurface(int width, int height, int format);

const Surface ConvertSurface(const Surface &source, int format);
const Surface ConvertSurface(const Surface &source, const SDL_PixelFormat &format);

const Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat &format);
const Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, int format);

void SetColorKey(Surface &surface, uint32_t *key);
uint32_t GetColorKey(const Surface &surface);
bool HasColorKey(const Surface &surface);

void CopyColorKey(SDL_Surface *src, SDL_Surface *dst);

void BlitSurface(const Surface &source, const Rect &sourceRect, Surface &dest, const Rect &destRect);
void BlitSurfaceScaled(const Surface &source, const Rect &sourceRect, Surface &dest, const Rect &destRect);

void DrawRhombus(Surface &dst, const Rect &bounds, const Color &color);
void DrawFrame(Surface &dst, const Rect &frame, const Color &color);
void FillFrame(Surface &dst, const Rect &frame, const Color &color);

void DrawRhombus(SDL_Renderer &renderer, const Rect &bounds, const Color &color);
void DrawFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color);
void FillFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color);


void BlurSurface(Surface &dst, int radius);

void TransformSurface(const Surface &surface, Color(Color const&));

uint32_t ExtractPixel(const Surface &surface, const Point &coord);
uint32_t ExtractPixelLocked(const Surface &surface, const Point &coord);

/**
 * \brief Wrapper around reinterpret_cast on surface->pixels
 */

inline char* GetPixels(Surface &surface)
{
    return reinterpret_cast<char*>(surface->pixels);
}

inline char const* GetPixels(const Surface &surface)
{
    return reinterpret_cast<char const*>(surface->pixels);
}

#endif
