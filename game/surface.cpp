#include "surface.h"

#include <cassert>

#include <functional>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>

#include <SDL.h>

#include <game/color.h>
#include <game/rect.h>
#include <game/point.h>
#include <game/sdl_utils.h>

namespace
{
    struct null_surface_error : public std::invalid_argument
    {
         null_surface_error() throw() : std::invalid_argument("surface is null or invalid") {}
    };

    struct null_pixeldata_error : public std::invalid_argument
    {
         null_pixeldata_error() throw() : std::invalid_argument("pixel pointer is null") {}
    };
        
    void AddSurfaceRef(SDL_Surface *surface)
    {
        if(surface != nullptr) {
            ++surface->refcount;
        }
    }
    
    class TransformFunctor
    {
        const SDL_PixelFormat &mFormat;
        std::function<Color(Color const&)> mFunc;

    public:
        TransformFunctor(const SDL_PixelFormat &format, Color func(Color const&))
            : mFormat(format)
            , mFunc(func)
            { }

        void operator()(char *bytes, size_t size)
        {
            const char *end = bytes + size * mFormat.BytesPerPixel;
        
            while(bytes != end) {
                const uint32_t origPixel = GetPackedPixel(bytes, mFormat.BytesPerPixel);

                Color color;
                SDL_GetRGBA(origPixel, &mFormat, &color.r, &color.g, &color.b, &color.a);

                const Color result = mFunc(color);
                const uint32_t pixel = SDL_MapRGBA(&mFormat, result.r, result.g, result.b, result.a);
                SetPackedPixel(bytes, pixel, mFormat.BytesPerPixel);

                bytes += mFormat.BytesPerPixel;
            }
        }
    };
}

SurfaceAlphaModSetter::SurfaceAlphaModSetter(const Surface &src, int newAlphaMod)
    : surface(src)
{
    if(SDL_GetSurfaceAlphaMod(surface, &alphaMod) < 0) {
        throw sdl_error();
    }
    if(SDL_SetSurfaceAlphaMod(surface, newAlphaMod) < 0) {
        throw sdl_error();
    }
}

void SurfaceAlphaModSetter::Rollback()
{
    if(SDL_SetSurfaceAlphaMod(surface, alphaMod) < 0) {
        throw sdl_error();
    }
}

SurfaceAlphaModSetter::~SurfaceAlphaModSetter()
{
    try {
        Rollback();
    } catch(const std::exception &error) {
        std::cerr << "unable recover alpha modifier: " << error.what() << std::endl;
    }
}

SurfaceClipper::SurfaceClipper(const Surface &surface, const Rect &cliprect)
    : mObject(surface)
    , mOldClip()
{
    if(!mObject.Null()) {
        SDL_GetClipRect(mObject, &mOldClip);
        SDL_SetClipRect(mObject, &cliprect);
    }
}

SurfaceClipper::~SurfaceClipper() throw()
{
    if(!mObject.Null()) {
        SDL_SetClipRect(mObject, &mOldClip);
    }
}

SurfaceLocker::SurfaceLocker(const Surface &surface)
    : mObject(surface)
    , mLocked(false)
{
    if(!mObject.Null()) {
        if(SDL_MUSTLOCK(mObject)) {
            if(0 == SDL_LockSurface(mObject)) {
                mLocked = true;
            }
        }
    }
}

SurfaceLocker::~SurfaceLocker() throw()
{
    if(mLocked && !mObject.Null()) {
        SDL_UnlockSurface(mObject);
    }
}

Surface::~Surface()
{
    SDL_FreeSurface(mSurface);
}

Surface::Surface()
    : mSurface(nullptr)
{
}

Surface::Surface(SDL_Surface *s)
    : mSurface(s)
{
}

Surface::Surface(const Surface &that)
    : mSurface(that.mSurface)
{
    AddSurfaceRef(mSurface);
}

Surface& Surface::operator=(SDL_Surface *s)
{
    Assign(s);
    return *this;
}

Surface& Surface::operator=(const Surface &that)
{
    AddSurfaceRef(that.mSurface);
    Assign(that.mSurface);
    return *this;
}

void Surface::Assign(SDL_Surface *s)
{
    SDL_Surface *tmp = mSurface;
    mSurface = s;

    /** SDL_FreeSurface decreases reference counter **/
    SDL_FreeSurface(tmp);
}

bool Surface::Null() const
{
    return (mSurface == nullptr);
}

SDL_Surface* Surface::operator->() const
{
    return mSurface;
}

Surface::operator SDL_Surface*() const
{
    return mSurface;
}

bool Surface::operator!() const
{
    return Null();
}

void Surface::Reset(SDL_Surface *surface)
{
    AddSurfaceRef(surface);
    Assign(surface);
}

void SetColorKey(Surface &surface, uint32_t *key)
{
    if(key == nullptr) {
        if(SDL_SetColorKey(surface, SDL_FALSE, 0) < 0) {
            throw sdl_error();
        }
    } else {
        if(SDL_SetColorKey(surface, SDL_TRUE, *key) < 0) {
            throw sdl_error();
        }
    }
}

uint32_t GetColorKey(const Surface &surface)
{
    uint32_t key;
    if(SDL_GetColorKey(surface, &key) != 0) {
        throw std::logic_error("surface has no color key");
    }
    return key;
}

bool HasColorKey(const Surface &surface)
{
    uint32_t key;
    return (SDL_GetColorKey(surface, &key) == 0);
}

void CopyColorKey(SDL_Surface *src, SDL_Surface *dst)
{
    uint32_t colorkey;
    if(SDL_GetColorKey(src, &colorkey) == 0) {
        if(SDL_SetColorKey(dst, SDL_TRUE, colorkey) < 0) {
            throw sdl_error();
        }
    }
}

const Rect GetClipRect(const Surface &surface)
{
    return Rect(surface->clip_rect);
}

const Surface CreateSurface(int width, int height, const SDL_PixelFormat &format)
{
    const uint32_t rmask = format.Rmask;
    const uint32_t gmask = format.Gmask;
    const uint32_t bmask = format.Bmask;
    const uint32_t amask = format.Amask;
    const int bpp = format.BitsPerPixel;
    Surface tmp;
    tmp = SDL_CreateRGBSurface(0, width, height, bpp, rmask, gmask, bmask, amask);
    if(!tmp) {
        throw sdl_error();
    }
    return tmp;
}

const Surface CreateSurface(int width, int height, int format)
{
    uint32_t rmask;
    uint32_t gmask;
    uint32_t bmask;
    uint32_t amask;
    int bpp;

    if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
        throw sdl_error();
    }
    
    Surface tmp;
    tmp = SDL_CreateRGBSurface(0, width, height, bpp, rmask, gmask, bmask, amask);
    if(!tmp) {
        throw sdl_error();
    }
    return tmp;
}

const Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat &format)
{
    const uint32_t rmask = format.Rmask;
    const uint32_t gmask = format.Gmask;
    const uint32_t bmask = format.Bmask;
    const uint32_t amask = format.Amask;
    const int bpp = format.BitsPerPixel;
    
    if(pixels != nullptr) {
        Surface tmp;
        tmp = SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
        if(!tmp) {
            throw sdl_error();
        }
        return tmp;
    }
    throw null_pixeldata_error();
}

const Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, int format)
{
    if(pixels != nullptr) {
        uint32_t rmask;
        uint32_t gmask;
        uint32_t bmask;
        uint32_t amask;
        int bpp;

        if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
            throw sdl_error();
        }
        
        Surface tmp;
        tmp = SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
        if(!tmp) {
            throw sdl_error();
        }
        return tmp;
    }
    throw null_pixeldata_error();
}

const Surface ConvertSurface(const Surface &source, int format)
{
    Surface tmp;
    tmp = SDL_ConvertSurfaceFormat(source, format, 0);
    
    if(!tmp) {
        throw sdl_error();
    }
    
    return tmp;
}

const Surface ConvertSurface(const Surface &source, const SDL_PixelFormat &format)
{
    Surface tmp;
    tmp = SDL_ConvertSurface(source, &format, 0);
    
    if(!tmp) {
        throw sdl_error();
    }
    
    return tmp;
}

void BlitSurface(const Surface &source, const Rect &sourceRect, Surface &dest, const Rect &destRect)
{
    // tempRect would be modified by SDL_BlitSurface
    Rect tempRect(destRect);
    
    if(SDL_BlitSurface(source, &sourceRect, dest, &tempRect) < 0) {
        throw sdl_error();
    }
}

void BlitSurfaceScaled(const Surface &source, const Rect &sourceRect, Surface &dest, const Rect &destRect)
{
    // see BlitSurface
    Rect tempRect(destRect);
    if(SDL_BlitScaled(source, &sourceRect, dest, &tempRect) < 0) {
        throw sdl_error();
    }
}

void TransformSurface(Surface &surface, Color func(Color const&))
{
    if(!surface) {
        throw null_surface_error();
    }
    
    const SurfaceLocker lock(surface);

    const int width = SurfaceWidth(surface);
    const int rowStride = SurfaceRowStride(surface);
    char *const bytes = SurfaceData(surface);

    TransformFunctor transform(SurfaceFormat(surface), func);
    
    for(int y = 0; y < SurfaceHeight(surface); ++y) {
        char *const data = bytes + rowStride * y;
        transform(data, width);
    }
}

uint32_t ExtractPixel(const Surface &surface, const Point &coord)
{
    if(!surface) {
        throw null_surface_error();
    }

    if(coord.x < 0 || coord.y < 0 || coord.x >= SurfaceWidth(surface) || coord.y >= SurfaceHeight(surface)) {
        throw std::invalid_argument("coord out of surface bounds");
    }
    
    const SurfaceLocker lock(surface);
    return ExtractPixelLocked(surface, coord);
}

uint32_t ExtractPixelLocked(const Surface &surface, const Point &coord)
{
    return GetPackedPixel(
        SurfaceData(surface)
        + coord.y * SurfaceRowStride(surface)
        + coord.x * SurfacePixelStride(surface), SurfacePixelStride(surface));
}

bool HasPalette(const Surface &surface)
{
    if(!surface) {
        throw null_surface_error();
    }
    return IsIndexed(SurfaceFormat(surface));
}

bool IsIndexed(const SDL_PixelFormat &format)
{
    return SDL_ISPIXELFORMAT_INDEXED(format.format);
}

bool IsRGB(const SDL_PixelFormat &format)
{
    return !SDL_ISPIXELFORMAT_ALPHA(format.format);
}

bool IsARGB(const SDL_PixelFormat &format)
{
    return SDL_ISPIXELFORMAT_ALPHA(format.format);
}

int SurfaceWidth(const Surface &surface)
{
    return surface->w;
}

int SurfaceHeight(const Surface &surface)
{
    return surface->h;
}

SDL_PixelFormat const& SurfaceFormat(const Surface &surface)
{
    assert(surface->format != NULL);
    return *surface->format;
}

int SurfaceRowStride(const Surface &surface)
{
    return surface->pitch;
}

int SurfacePixelStride(const Surface &surface)
{
    assert(surface->format != NULL);
    return surface->format->BytesPerPixel;
}

char const* SurfaceData(const Surface &surface)
{
    return reinterpret_cast<char const*>(surface->pixels);
}

char* SurfaceData(Surface &surface)
{
    return reinterpret_cast<char*>(surface->pixels);
}

SDL_BlendMode GetSurfaceBlendMode(const Surface &surface)
{
    SDL_BlendMode mode;
    if(SDL_GetSurfaceBlendMode(surface, &mode) < 0) {
        throw sdl_error();
    }
    return mode;
}
