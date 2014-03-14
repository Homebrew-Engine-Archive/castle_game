#include "streamingtexture.h"

StreamingTexture::StreamingTexture(SDL_Renderer *renderer, int width, int height, Uint32 format)
    : m_width(width)
    , m_height(height)
    , m_locked(false)
    , m_renderer(renderer)
{
    if(!SDL_PixelFormatEnumToMasks(format, &m_bpp, &m_rmask, &m_gmask, &m_bmask, &m_amask)) {
        throw std::runtime_error(SDL_GetError());
    }
}

StreamingTexture::~StreamingTexture()
{
    if(m_texture != NULL)
        SDL_DestroyTexture(m_texture);
}

Surface StreamingTexture::Lock()
{
    int nativePitch;
    void *nativePixels;
    if(SDL_LockTexture(m_texture, NULL, &nativePixels, &nativePitch)) {
        std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << std::endl;
        return Surface();
    }

    Surface surface = SDL_CreateRGBSurfaceFrom(
        nativePixels,
        m_width,
        m_height,
        m_bpp,
        nativePitch,
        m_rmask,
        m_gmask,
        m_bmask,
        m_amask);
    
    if(surface.Null()) {
        std::cerr << "SDL_CreateRGBSurfaceFrom failed: " << SDL_GetError() << std::endl;
        SDL_UnlockTexture(m_texture);
    }

    m_locked = true;
    return surface;
}

void StreamingTexture::Unlock()
{
    if(m_locked) {
        SDL_UnlockTexture(m_texture);
        m_locked = false;
    } else {
        std::cerr << "Trying unlock not locked texture" << std::endl;
    }
}
