#include "surface.h"

#include <cassert>

#include <functional>
#include <iostream>
#include <memory>
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

    class ConvolveFunctor
    {
        std::vector<uint32_t> mBuffer;
        uint32_t *const mRedBuff;
        uint32_t *const mGreenBuff;
        uint32_t *const mBlueBuff;
        const int mRadius;
        const SDL_PixelFormat &mFormat;

    public:
        ConvolveFunctor(int radius, const SDL_PixelFormat &format, int bufferSize)
            : mBuffer(bufferSize * 3)
            , mRedBuff(mBuffer.data())
            , mGreenBuff(mBuffer.data() + bufferSize)
            , mBlueBuff(mBuffer.data() + bufferSize * 2)
            , mRadius(radius)
            , mFormat(format)
            { }
        
        void operator()(char *bytes, int length, int stride)
        {    
            uint32_t redAccum = 0;
            uint32_t greenAccum = 0;
            uint32_t blueAccum = 0;

            for(int i = 0; i < length; ++i) {
                const uint32_t pixel = GetPackedPixel(bytes + i * stride, mFormat.BytesPerPixel);

                uint8_t r;
                uint8_t g;
                uint8_t b;
                SDL_GetRGB(pixel, &mFormat, &r, &g, &b);

                redAccum += r;
                greenAccum += g;
                blueAccum += b;

                mRedBuff[i] = redAccum;
                mGreenBuff[i] = greenAccum;
                mBlueBuff[i] = blueAccum;
            }

            for(int i = 0; i < length; ++i) {
                const size_t minIndex = std::max(0, i - mRadius);
                const size_t maxIndex = std::min(i + mRadius, length - 1);

                const uint32_t red = mRedBuff[maxIndex] - mRedBuff[minIndex];
                const uint32_t green = mGreenBuff[maxIndex] - mGreenBuff[minIndex];
                const uint32_t blue = mBlueBuff[maxIndex] - mBlueBuff[minIndex];
                const uint32_t count = maxIndex - minIndex;

                const uint32_t pixel = SDL_MapRGB(&mFormat, red / count, green / count, blue / count);
                SetPackedPixel(bytes + i * stride, pixel, mFormat.BytesPerPixel);
            }
        }
    };
}

SurfaceColorModSetter::SurfaceColorModSetter(const Surface &src, const Color &color)
{
    if(SDL_GetSurfaceColorMod(src, &redMod, &greenMod, &blueMod) < 0) {
        throw sdl_error();
    }
    if(SDL_SetSurfaceColorMod(src, color.r, color.g, color.b) < 0) {
        throw sdl_error();
    }
    surface = src;
}

SurfaceColorModSetter::~SurfaceColorModSetter()
{
    if(!surface.Null()) {
        SDL_SetSurfaceColorMod(surface, redMod, greenMod, blueMod);
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

SurfaceLocker::~SurfaceLocker()
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

const Surface CreateSurfaceView(Surface &src, const Rect &clip)
{
    if(SDL_MUSTLOCK(src)) {
        // can we deal with it?
        throw std::invalid_argument("SurfaceView might not be created from RLEaccel surface");
    }

    const Rect cropped =
        IntersectRects(
            Normalized(clip),
            Rect(src));

    char *const pixels = GetPixels(src)
        + cropped.y * src->pitch
        + cropped.x * src->format->BytesPerPixel;

    Surface tmp;
    tmp = CreateSurfaceFrom(pixels, cropped.w, cropped.h, src->pitch, *src->format);
    if(!tmp) {
        throw sdl_error();
    }
    CopyColorKey(src, tmp);
    
    return tmp;
}

SurfaceView::SurfaceView(Surface &src, const Rect &clip)
    : mSurface(CreateSurfaceView(src, clip))
    , mParentRef(src)
{
}

// \todo there is something wrong with const-specifier
// problem arises to SDL_CreateRGBSurfaceFrom which asks for non-const pixels
SurfaceView::SurfaceView(const Surface &src, const Rect &clip)
    : SurfaceView(const_cast<Surface&>(src), clip)
{
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
    // SDL_BlitSurface demands non-const pointer to rect. For what!?
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

void DrawFrame(Surface &dst, const Rect &frame, const Color &color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    if(!render) {
        throw sdl_error();
    }

    DrawFrame(*render, frame, color);
}

void FillFrame(Surface &dst, const Rect &frame, const Color &color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    if(!render) {
        throw sdl_error();
    }

    FillFrame(*render, frame, color);
}

void DrawRhombus(Surface &dst, const Rect &bounds, const Color &color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    if(!render) {
        throw sdl_error();
    }

    DrawRhombus(*render, bounds, color);
}

void DrawFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color)
{
    SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
    if(SDL_RenderDrawRect(&renderer, &frame) < 0) {
        throw sdl_error();
    }
}

void FillFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color)
{
    SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
    if(SDL_RenderFillRect(&renderer, &frame) < 0) {
        throw sdl_error();
    }
}

void DrawRhombus(SDL_Renderer &renderer, const Rect &bounds, const Color &color)
{
    constexpr int numPoints = 5;

    const auto x1 = bounds.x;
    const auto y1 = bounds.y;
    const auto x2 = bounds.x + bounds.w;
    const auto y2 = bounds.y + bounds.h;

    const auto centerX = (x1 + x2) / 2;
    const auto centerY = (y1 + y2) / 2;
    
    const Point points[] = {
        Point(x1, centerY),
        Point(centerX, y1),
        Point(x2, centerY),
        Point(centerX, y2),
        Point(x1, centerY)
    };
    
    SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);    
    if(SDL_RenderDrawLines(&renderer, &points[0], numPoints) < 0) {
        throw sdl_error();
    }
}

void TransformSurface(Surface &dst, Color func(Color const&))
{
    if(!dst) {
        throw null_surface_error();
    }
    
    const SurfaceLocker lock(dst);
    char *const bytes = GetPixels(dst);
    TransformFunctor transform(*dst->format, func);
    
    for(int i = 0; i < dst->h; ++i) {
        char *const data = bytes + i * dst->pitch;
        const auto count = dst->w;
        transform(data, count);
    }
}

void BlurSurface(Surface &dst, int radius)
{
    if(!dst) {
        throw null_surface_error();
    }

    const auto buffSize = std::max(dst->w, dst->h);
    ConvolveFunctor convolve(radius, *dst->format, buffSize);
    
    if(radius < 1 || radius > buffSize) {
        throw std::invalid_argument("inproper convolution radius");
    }

    const SurfaceLocker lock(dst);
    char *const bytes = GetPixels(dst);
    
    /** Per row convolution **/
    for(int y = 0; y < dst->h; ++y) {
        char *const data = bytes + y * dst->pitch;
        const auto count = dst->w;
        const auto stride = dst->format->BytesPerPixel;
        convolve(data, count, stride);
    }

    /** Per column convolution **/
    for(int x = 0; x < dst->w; ++x) {
        const auto stride = dst->pitch;
        const auto count = dst->h;
        char *const data = bytes + x * dst->format->BytesPerPixel;
        convolve(data, count, stride);
    }
}

uint32_t ExtractPixel(const Surface &surface, const Point &coord)
{
    if(!surface) {
        throw null_surface_error();
    }

    if(coord.x < 0 || coord.y < 0 || coord.x >= surface->w || coord.y >= surface->h) {
        throw std::invalid_argument("coord out of surface bounds");
    }
    
    const SurfaceLocker lock(surface);
    return ExtractPixelLocked(surface, coord);
}

uint32_t ExtractPixelLocked(const Surface &surface, const Point &coord)
{
    return GetPackedPixel(GetPixels(surface) + coord.y * surface->pitch + coord.x * surface->format->BytesPerPixel, surface->format->BytesPerPixel);
}

bool HasPalette(const Surface &surface)
{
    if(!surface) {
        throw null_surface_error();
    }
    return SDL_ISPIXELFORMAT_INDEXED(surface->format->format);
}
