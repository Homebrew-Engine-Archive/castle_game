#ifndef STREAMING_TEXTURE_H_
#define STREAMING_TEXTURE_H_

#include <iostream>
#include "SDL.h"

#include "surface.h"

class StreamingTexture
{
    int m_width;
    int m_height;
    Uint32 m_rmask;
    Uint32 m_gmask;
    Uint32 m_bmask;
    Uint32 m_amask;
    int m_bpp;
    bool m_locked;
    SDL_Texture *m_texture;
    SDL_Renderer *m_renderer;
    
public:
    explicit StreamingTexture(SDL_Renderer *renderer, int width, int height, Uint32 format);
    ~StreamingTexture();

    Surface Lock();
    void Unlock();
    
};

#endif
