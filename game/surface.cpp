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
#include <game/make_unique.h>

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
    
    void MapBuffer(char *bytes, size_t size, const SDL_PixelFormat *format, SDL_Color func(uint8_t, uint8_t, uint8_t, uint8_t))
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
    
    void ConvolveBuffer(char *bytes, int length, int stride, const SDL_PixelFormat *format, uint32_t *redBuff, uint32_t *greenBuff, uint32_t *blueBuff, int radius)
    {
        uint32_t redAccum = 0;
        uint32_t greenAccum = 0;
        uint32_t blueAccum = 0;

        for(int i = 0; i < length; ++i) {
            const uint32_t pixel = GetPackedPixel(bytes + i * stride, format->BytesPerPixel);

            uint8_t r;
            uint8_t g;
            uint8_t b;
            SDL_GetRGB(pixel, format, &r, &g, &b);
            
            redAccum += r;
            greenAccum += g;
            blueAccum += b;
            
            redBuff[i] = redAccum;
            greenBuff[i] = greenAccum;
            blueBuff[i] = blueAccum;
        }

        for(int i = 0; i < length; ++i) {
            const size_t minIndex = std::max(0, i - radius);
            const size_t maxIndex = std::min(i + radius, length - 1);

            const uint32_t red = redBuff[maxIndex] - redBuff[minIndex];
            const uint32_t green = greenBuff[maxIndex] - greenBuff[minIndex];
            const uint32_t blue = blueBuff[maxIndex] - blueBuff[minIndex];
            const uint32_t count = maxIndex - minIndex;

            const uint32_t pixel = SDL_MapRGB(format, red / count, green / count, blue / count);
            SetPackedPixel(bytes + i * stride, pixel, format->BytesPerPixel);
        }
    }    
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

Surface &Surface::operator=(SDL_Surface *s)
{
    Assign(s);
    return *this;
}

Surface &Surface::operator=(const Surface &that)
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

SDL_Surface *Surface::operator->() const
{
    return mSurface;
}

Surface::operator SDL_Surface *() const
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

SurfaceView::SurfaceView(Surface &src, const SDL_Rect *roi)
{
    if(!src) {
        return;
    }
    
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    if(roi != nullptr) {
        x = roi->x;
        y = roi->y;
        width = roi->w;
        height = roi->h;
    } else {
        x = 0;
        y = 0;
        width = src->w;
        height = src->h;
    }    

    const int bytesPerPixel = src->format->BytesPerPixel;
    
    char *pixels = GetPixels(src)
        + y * src->pitch
        + x * bytesPerPixel;
    
    Surface tmp = CreateSurfaceFrom(pixels, width, height, src->pitch, src->format);
    if(!tmp) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
    
    CopyColorKey(src, tmp);

    mSurface = tmp;
    AddSurfaceRef(mSurface);
    
    mReferer = src;
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
        throw std::invalid_argument("CreateSurface: passed nullptr format");
    }

    const uint32_t rmask = format->Rmask;
    const uint32_t gmask = format->Gmask;
    const uint32_t bmask = format->Bmask;
    const uint32_t amask = format->Amask;
    const int bpp = format->BitsPerPixel;

    return SDL_CreateRGBSurface(NoFlags, width, height, bpp, rmask, gmask, bmask, amask);
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
    
    return SDL_CreateRGBSurface(NoFlags, width, height, bpp, rmask, gmask, bmask, amask);
}

Surface CreateSurfaceFrom(void *pixels, int width, int height, int pitch, const SDL_PixelFormat *format)
{
    if(pixels == nullptr) {
        std::ostringstream oss;
        oss << BOOST_CURRENT_FUNCTION << ": passed nullptr";
        throw std::invalid_argument(oss.str());
    }
    
    if(format == nullptr) {
        std::ostringstream oss;
        oss << BOOST_CURRENT_FUNCTION << ": passed nullptr format";
        throw std::invalid_argument(oss.str());
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
        std::ostringstream oss;
        oss << BOOST_CURRENT_FUNCTION << ": passed nullptr pixels";
        throw std::invalid_argument(oss.str());
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

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect)
{
    if(SDL_BlitSurface(src, srcrect, dst, dstrect) < 0) {
        Fail(BOOST_CURRENT_FUNCTION, SDL_GetError());
    }
}

void DrawFrame(Surface &dst, const SDL_Rect &dstrect, SDL_Color color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(render.get(), color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(render.get(), &dstrect);
}

void FillFrame(Surface &dst, const SDL_Rect &dstrect, SDL_Color color)
{
    RendererPtr render(SDL_CreateSoftwareRenderer(dst));
    SDL_SetRenderDrawBlendMode(render.get(), SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(render.get(), color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(render.get(), &dstrect);
}

SDL_Rect SurfaceBounds(const Surface &src)
{
    return (src.Null()
            ? MakeEmptyRect()
            : MakeRect(src->w, src->h));
}

void MapSurface(Surface &dst, SDL_Color func(uint8_t, uint8_t, uint8_t, uint8_t))
{
    if(!dst) {
        return;
    }
    
    const SurfaceLocker lock(dst);

    char *data = GetPixels(dst);
    const SDL_PixelFormat *fmt = dst->format;

    for(int y = 0; y < dst->h; ++y) {
        MapBuffer(data, dst->w, fmt, func);
        data += dst->pitch;
    }
}

void BlurSurface(Surface &dst, int radius)
{
    const SurfaceLocker lock(dst);

    if(!dst) {
        return;
    }

    const int bufferSize = std::max(dst->w, dst->h);
    std::vector<uint32_t> buffer(bufferSize * 3);

    uint32_t *const redBuffer = &buffer[bufferSize*0];
    uint32_t *const greenBuffer = &buffer[bufferSize*1];
    uint32_t *const blueBuffer = &buffer[bufferSize*2];

    char *const bytes = GetPixels(dst);

    const int bytesPP = dst->format->BytesPerPixel;

    for(int y = 0; y < dst->h; ++y) {
        ConvolveBuffer(bytes + y * dst->pitch, dst->w, bytesPP, dst->format, redBuffer, greenBuffer, blueBuffer, radius);
    }

    for(int x = 0; x < dst->w; ++x) {
        ConvolveBuffer(bytes + x * bytesPP, dst->h, dst->pitch, dst->format, redBuffer, greenBuffer, blueBuffer, radius);
    }
}
