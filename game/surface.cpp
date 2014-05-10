#include "surface.h"

#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <vector>

#include <SDL.h>

#include <boost/current_function.hpp>

#include <game/sdl_utils.h>

namespace
{
    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }
    
    void AddSurfaceRef(SDL_Surface *surface)
    {
        if(surface != nullptr) {
            ++surface->refcount;
        }
    }
    
    void TransformBuffer(char *bytes, size_t size, const SDL_PixelFormat *format, SDL_Color func(uint8_t, uint8_t, uint8_t, uint8_t))
    {
        const char *end = bytes + size * format->BytesPerPixel;
        
        while(bytes != end) {
            const uint32_t origPixel = GetPackedPixel(bytes, format->BytesPerPixel);

            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
            SDL_GetRGBA(origPixel, format, &r, &g, &b, &a);
        
            const SDL_Color result = func(r, g, b, a);

            const uint32_t pixel = SDL_MapRGBA(format, result.r, result.g, result.b, result.a);
            SetPackedPixel(bytes, pixel, format->BytesPerPixel);
            
            bytes += format->BytesPerPixel;
        }
    }

    struct ConvolveFunctor
    {
        std::vector<uint32_t> mBuffer;
        uint32_t *const mRedBuff;
        uint32_t *const mGreenBuff;
        uint32_t *const mBlueBuff;
        const int mRadius;
        const SDL_PixelFormat *mFormat;

        ConvolveFunctor(int radius, const SDL_PixelFormat *format, int bufferSize)
            : mBuffer(bufferSize * 3)
            , mRedBuff(&mBuffer[bufferSize * 0])
            , mGreenBuff(&mBuffer[bufferSize * 1])
            , mBlueBuff(&mBuffer[bufferSize * 2])
            , mRadius(radius)
            , mFormat(format)
            { }
        
        void operator()(char *bytes, int length, int stride)
        {    
            uint32_t redAccum = 0;
            uint32_t greenAccum = 0;
            uint32_t blueAccum = 0;

            for(int i = 0; i < length; ++i) {
                const uint32_t pixel = GetPackedPixel(bytes + i * stride, mFormat->BytesPerPixel);

                uint8_t r;
                uint8_t g;
                uint8_t b;
                SDL_GetRGB(pixel, mFormat, &r, &g, &b);

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

                const uint32_t pixel = SDL_MapRGB(mFormat, red / count, green / count, blue / count);
                SetPackedPixel(bytes + i * stride, pixel, mFormat->BytesPerPixel);
            }
        }
    };
}

SurfaceLocker::SurfaceLocker(const Surface &surface)
    : mObject(surface)
    , mLocked(false)
{
    if(!mObject.Null()) {
        if(SDL_MUSTLOCK(mObject)) {
            if(0 == SDL_LockSurface(mObject))
                mLocked = true;
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

bool Surface::operator==(const Surface &that)
{
    return that.mSurface == mSurface;
}

void Surface::Assign(SDL_Surface *s)
{
    // SDL_FreeSurface manages refcount by itself
    // Suddenly.
    SDL_FreeSurface(mSurface);
    mSurface = s;
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

void Surface::reset(SDL_Surface *surface)
{
    AddSurfaceRef(surface);
    Assign(surface);
}

SurfaceView::SurfaceView(Surface &src, const SDL_Rect &clip)
{
    if(!src) {
        Fail(BOOST_CURRENT_FUNCTION, "Null surface");
    }

    if(SDL_MUSTLOCK(src)) {
        // can we deal with it?
        Fail(BOOST_CURRENT_FUNCTION, "RLEaccel installed on source surface");
    }
    
    char *pixels = GetPixels(src)
        + clip.y * src->pitch
        + clip.x * src->format->BytesPerPixel;
    
    Surface tmp = CreateSurfaceFrom(pixels, clip.w, clip.h, src->pitch, src->format);
    if(!tmp) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
    
    CopyColorKey(src, tmp);

    mReferer = src;
    mSurface = tmp;
    AddSurfaceRef(mSurface);
}

void CopyColorKey(SDL_Surface *src, SDL_Surface *dst)
{
    uint32_t colorkey = 0;
    if(SDL_GetColorKey(src, &colorkey) == 0) {
        if(SDL_SetColorKey(dst, SDL_TRUE, colorkey) < 0) {
            Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
        }
    }
}

Surface CreateSurface(int width, int height, const SDL_PixelFormat *format)
{
    if(format == nullptr) {
        Fail(BOOST_CURRENT_FUNCTION, "Null format");
    }

    const uint32_t rmask = format->Rmask;
    const uint32_t gmask = format->Gmask;
    const uint32_t bmask = format->Bmask;
    const uint32_t amask = format->Amask;
    const int bpp = format->BitsPerPixel;

    return SDL_CreateRGBSurface(0, width, height, bpp, rmask, gmask, bmask, amask);
}

Surface CreateSurface(int width, int height, int format)
{
    uint32_t rmask;
    uint32_t gmask;
    uint32_t bmask;
    uint32_t amask;
    int bpp;

    if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
    
    return SDL_CreateRGBSurface(0, width, height, bpp, rmask, gmask, bmask, amask);
}

Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat *format)
{
    if(pixels == nullptr) {
        Fail(BOOST_CURRENT_FUNCTION, "Null pixels");
    }
    
    if(format == nullptr) {
        Fail(BOOST_CURRENT_FUNCTION, "Null format");
    }
        
    const uint32_t rmask = format->Rmask;
    const uint32_t gmask = format->Gmask;
    const uint32_t bmask = format->Bmask;
    const uint32_t amask = format->Amask;
    const int bpp = format->BitsPerPixel;

    return SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
}

Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, int format)
{
    if(pixels == nullptr) {
        Fail(BOOST_CURRENT_FUNCTION, "Null pixels");
    }
        
    uint32_t rmask;
    uint32_t gmask;
    uint32_t bmask;
    uint32_t amask;
    int bpp;

    if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask)) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }

    return SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
}

SDL_Rect FindCropRect(const Surface &surface)
{
    return SurfaceBounds(surface);
}

void BlitSurface(const Surface &src, const SDL_Rect &srcrect, Surface &dst, const SDL_Rect &dstrect)
{
    if(SDL_BlitSurface(src, &srcrect, dst, &const_cast<SDL_Rect&>(dstrect)) < 0) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
}

void DrawFrame(Surface &dst, const SDL_Rect &dstrect, const SDL_Color &color)
{
    if(!dst) {
        Fail(BOOST_CURRENT_FUNCTION, "Null surface");
    }
    
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(render.get(), color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(render.get(), &dstrect);
}

void FillFrame(Surface &dst, const SDL_Rect &dstrect, const SDL_Color &color)
{
    if(!dst) {
        Fail(BOOST_CURRENT_FUNCTION, "Null surface");
    }
    
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(render.get(), color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(render.get(), &dstrect);
}

SDL_Rect SurfaceBounds(const Surface &src)
{
    return ((!src) ? MakeEmptyRect() : MakeRect(src->w, src->h));
}

void TransformSurface(Surface &dst, SDL_Color func(uint8_t, uint8_t, uint8_t, uint8_t))
{
    if(!dst) {
        Fail(BOOST_CURRENT_FUNCTION, "Null surface");
    }
    
    const SurfaceLocker lock(dst);

    char *bytes = GetPixels(dst);

    for(int y = 0; y < dst->h; ++y) {
        TransformBuffer(bytes, dst->w, dst->format, func);
        bytes += dst->pitch;
    }
}

void BlurSurface(Surface &dst, int radius)
{
    if(!dst) {
        Fail(BOOST_CURRENT_FUNCTION, "Null surface");
    }
    
    const SurfaceLocker lock(dst);
    
    char *const bytes = GetPixels(dst);
    const int bytesPP = dst->format->BytesPerPixel;
    ConvolveFunctor convolve(radius, dst->format, std::max(dst->w, dst->h));
    
    for(int y = 0; y < dst->h; ++y) {
        convolve(bytes + y * dst->pitch, dst->w, bytesPP);
    }

    for(int x = 0; x < dst->w; ++x) {
        convolve(bytes + x * bytesPP, dst->h, dst->pitch);
    }
}

bool HasPalette(const Surface &surface)
{
    return SDL_ISPIXELFORMAT_INDEXED(surface->format->format);
}
