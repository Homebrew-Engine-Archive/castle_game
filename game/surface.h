#ifndef SURFACE_H_
#define SURFACE_H_

#include <SDL.h>

#include <game/rect.h>

namespace core
{
    class Point;
    class Color;
}

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
    SurfaceLocker(SurfaceLocker const&) = delete;
    SurfaceLocker &operator=(SurfaceLocker const&) = delete;
    ~SurfaceLocker() throw();
};

class SurfaceAlphaModSetter final
{
    Surface surface;
    uint8_t alphaMod;
public:
    SurfaceAlphaModSetter(const Surface &src, int alpha);

    void Rollback();
    ~SurfaceAlphaModSetter();
};

class SurfaceClipper
{
    const Surface &mObject;
    core::Rect mOldClip;
    
public:
    SurfaceClipper(const Surface &surface, const core::Rect &cliprect);
    ~SurfaceClipper() throw();
};

bool HasPalette(const Surface &surface);
bool IsIndexed(const SDL_PixelFormat &format);
bool IsRGB(const SDL_PixelFormat &format);
bool IsARGB(const SDL_PixelFormat &format);

int SurfaceWidth(const Surface &surface);
int SurfaceHeight(const Surface &surface);
int SurfaceRowStride(const Surface &surface);
int SurfacePixelStride(const Surface &surface);
char const* SurfaceData(const Surface &surface);
char* SurfaceData(Surface &surface);
SDL_PixelFormat const& SurfaceFormat(const Surface &surface);

const Surface CreateSurface(int width, int height, const SDL_PixelFormat &format);
const Surface CreateSurface(int width, int height, int format);

const Surface ConvertSurface(const Surface &source, int format);
const Surface ConvertSurface(const Surface &source, const SDL_PixelFormat &format);

const Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat &format);
const Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, int format);

void SetColorKey(Surface &surface, uint32_t *key);
uint32_t GetColorKey(const Surface &surface);
bool HasColorKey(const Surface &surface);

const core::Rect GetClipRect(const Surface &surface);
SDL_BlendMode GetSurfaceBlendMode(const Surface &surface);

void CopyColorKey(SDL_Surface *src, SDL_Surface *dst);

void BlitSurface(const Surface &source, const core::Rect &sourceRect, Surface &dest, const core::Rect &destRect);
void BlitSurfaceScaled(const Surface &source, const core::Rect &sourceRect, Surface &dest, const core::Rect &destRect);

void TransformSurface(const Surface &surface, core::Color(core::Color const&));

uint32_t ExtractPixel(const Surface &surface, const core::Point &coord);
uint32_t ExtractPixelLocked(const Surface &surface, const core::Point &coord);

#endif
