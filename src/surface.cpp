#include "surface.h"
#include "geometry.h"
#include "macrosota.h"
#include <memory>
#include <algorithm>
#include <stdexcept>

SurfaceLocker::SurfaceLocker(const Surface &surface)
    : object(surface)
    , locked(false)
{
    if(!object.Null()) {
        if(SDL_MUSTLOCK(object)) {
            if(0 == SDL_LockSurface(object))
                locked = true;
        }
    }
}

SurfaceLocker::~SurfaceLocker()
{
    if(locked && !object.Null()) {
        SDL_UnlockSurface(object);
    }
}

ColorKeyLocker::ColorKeyLocker(const Surface &surface, bool enabled, Uint32 color)
    : object(surface)
{
    if(!object.Null()) {
        oldEnabled =
            (0 == SDL_GetColorKey(object, &oldColor));
        SDL_SetColorKey(object, enabled, color);
    }
}

ColorKeyLocker::~ColorKeyLocker()
{
    if(!object.Null()) {
        SDL_SetColorKey(object, oldEnabled, oldColor);
    }
}

Surface::~Surface()
{
    SDL_FreeSurface(m_surface);
}

Surface::Surface()
    : m_surface(NULL)
{
}

Surface::Surface(SDL_Surface *s)
    : m_surface(s)
{
}

Surface::Surface(const Surface &that)
    : m_surface(that.m_surface)
{
    AddRef(m_surface);
}

Surface &Surface::operator=(SDL_Surface *s)
{
    Assign(s);
    return *this;
}

Surface &Surface::operator=(const Surface &that)
{
    AddRef(that.m_surface);
    Assign(that.m_surface);
    return *this;
}

bool Surface::operator==(const Surface &that)
{
    return that.m_surface == m_surface;
}

void Surface::Assign(SDL_Surface *s)
{
    // SDL_FreeSurface manages refcount by itself
    // Suddenly.
    SDL_FreeSurface(m_surface);
    m_surface = s;
}

void Surface::AddRef(SDL_Surface *surf)
{
    if(surf != NULL)
        ++surf->refcount;
}

bool Surface::Null() const
{
    return (m_surface == NULL);
}

SDL_Surface *Surface::operator->() const
{
    return m_surface;
}

Surface::operator SDL_Surface *() const
{
    return m_surface;
}

void Surface::reset()
{
    Assign(NULL);
}

SurfaceROI::~SurfaceROI()
{
    SDL_FreeSurface(m_referer);
}

SurfaceROI::SurfaceROI(const Surface &src, const SDL_Rect *roi)
    : m_referer(NULL)
{
    if(src.Null())
        return;
    
    Uint32 x;
    Uint32 y;
    Uint32 width;
    Uint32 height;

    if(roi != NULL) {
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
    
    Uint32 depth = src->format->BitsPerPixel;
    Uint32 rmask = src->format->Rmask;
    Uint32 gmask = src->format->Gmask;
    Uint32 bmask = src->format->Bmask;
    Uint32 amask = src->format->Amask;
    Uint32 bytesPerPixel = src->format->BytesPerPixel;
    
    Uint8 *pixels = (Uint8 *)src->pixels
        + y * src->pitch
        + x * bytesPerPixel;
    
    m_surface = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, depth, src->pitch,
        rmask, gmask, bmask, amask);
    
    if(m_surface == NULL)
        return;
    
    Uint32 colorkey;
    if(SDL_GetColorKey(src, &colorkey) == 0) {
        SDL_SetColorKey(m_surface, SDL_TRUE, colorkey);
    }

    m_referer = src;
    AddRef(m_referer);
}

void CopySurfaceColorKey(const Surface &src, Surface &dst)
{
    Uint32 colorkey;
    bool enabled = (0 == SDL_GetColorKey(src, &colorkey));

    SDL_SetColorKey(dst, enabled, colorkey);
}

Surface CopySurface(const Surface &src, const SDL_Rect *srcrect)
{
    Uint32 width;
    Uint32 height;
    if(srcrect == NULL) {
        width = src->w;
        height = src->h;
    } else {
        width = srcrect->w;
        height = srcrect->h;
    }

    Surface dst = CopySurfaceFormat(src, width, height);

    Uint32 key;
    SDL_GetColorKey(src, &key);

    ColorKeyLocker lock(src, SDL_FALSE, key);
    BlitSurface(src, srcrect, dst, NULL);

    return dst;
}

Surface CopySurfaceFormat(const Surface &src, int width, int height)
{
    if(src.Null())
        return Surface();
    
    Uint32 depth = src->format->BitsPerPixel;
    Uint32 rmask = src->format->Rmask;
    Uint32 gmask = src->format->Gmask;
    Uint32 bmask = src->format->Bmask;
    Uint32 amask = src->format->Amask;
    
    Surface dst = SDL_CreateRGBSurface(
        NO_FLAGS, width, height, depth,
        rmask, gmask, bmask, amask);
    if(dst.Null())
        throw std::runtime_error(SDL_GetError());

    CopySurfaceColorKey(src, dst);

    return dst;
}

Surface SubSurface(Surface &src, const SDL_Rect *rect)
{
    Uint32 x = 0;
    Uint32 y = 0;
    Uint32 width = src->w;
    Uint32 height = src->h;
    if(rect != NULL) {
        x = rect->x;
        y = rect->y;
        width = rect->x;
        height = rect->y;
    }
    
    Uint32 depth = src->format->BitsPerPixel;
    Uint32 rmask = src->format->Rmask;
    Uint32 gmask = src->format->Gmask;
    Uint32 bmask = src->format->Bmask;
    Uint32 amask = src->format->Amask;
    Uint32 bytesPerPixel = src->format->BytesPerPixel;
    
    Uint8 *pixels = (Uint8 *)src->pixels
        + y * src->pitch
        + x * bytesPerPixel;
    
    Surface dst = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, depth, src->pitch,
        rmask, gmask, bmask, amask);
    if(dst.Null()) {
        throw std::runtime_error(SDL_GetError());
    }
    
    Uint32 colorkey;
    if(0 == SDL_GetColorKey(src, &colorkey)) {
        SDL_SetColorKey(dst, SDL_TRUE, colorkey);
    }
    
    return dst;
}

void BlitSurface(const Surface &src, const SDL_Rect *srcrect, Surface &dst, SDL_Rect *dstrect)
{
    if(SDL_BlitSurface(src, srcrect, dst, dstrect)) {
        throw std::runtime_error(SDL_GetError());
    }
}

void DrawFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    SDL_Renderer *render = SDL_CreateSoftwareRenderer(dst);
    SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

    Uint8 r, g, b, a;
    SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(render, r, g, b, a);
    SDL_RenderDrawRect(render, dstrect);
    SDL_DestroyRenderer(render);
}

void FillFrame(Surface &dst, const SDL_Rect *dstrect, Uint32 color)
{
    SDL_Renderer *render = SDL_CreateSoftwareRenderer(dst);
    SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

    Uint8 r, g, b, a;
    SDL_GetRGBA(color, dst->format, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(render, r, g, b, a);
    SDL_RenderFillRect(render, dstrect);
    SDL_DestroyRenderer(render);
}

SDL_Rect SurfaceBounds(const Surface &src)
{
    return (src.Null()
            ? MakeEmptyRect()
            : MakeRect(src->w, src->h));
}

bool HasPalette(const Surface &surface)
{
    return !surface.Null()
        && surface->format != NULL
        && surface->format->BitsPerPixel == 8;
}

template<class Pixel>
static void TransformLine(
    void *pixels,
    size_t count,
    const SDL_PixelFormat *pf,
    const std::function<SDL_Color(Uint8, Uint8, Uint8, Uint8)> &fun)
{
    Pixel *first = reinterpret_cast<Pixel *>(pixels);
    Pixel *last = first + count;
    while(first != last) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(*first, pf, &r, &g, &b, &a);
        
        SDL_Color result = fun(r, g, b, a);
        *first = SDL_MapRGBA(pf, result.r, result.g, result.b, result.a);
        ++first;
    }
}

template<class Pixel>
static void TransformImpl(
    Surface &dst, const std::function<SDL_Color(Uint8, Uint8, Uint8, Uint8)> &fun)
{
    if(dst.Null())
        return;

    Uint8 *pixels = reinterpret_cast<Uint8 *>(dst->pixels);
    const SDL_PixelFormat *fmt = dst->format;
    
    for(int y = 0; y < dst->h; ++y) {
        TransformLine<Pixel>(pixels, dst->w, fmt, fun);
        pixels += dst->pitch;
    }
}

void MapSurface(
    Surface &dst, const std::function<SDL_Color(Uint8, Uint8, Uint8, Uint8)> &fun)
{
    if(dst.Null())
        return;
    
    SurfaceLocker lock(dst);

    switch(dst->format->BytesPerPixel) {
    case 1:
        TransformImpl<Uint8>(dst, fun);
        break;
    case 2:
        TransformImpl<Uint16>(dst, fun);
        break;
    case 3:
        // implement me
        break;
    case 4:
        TransformImpl<Uint32>(dst, fun);
        break;
    default:
        TransformImpl<Uint32>(dst, fun);
        break;
    }
}
