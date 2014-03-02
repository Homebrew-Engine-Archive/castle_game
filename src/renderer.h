#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <stdexcept>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <SDL2/SDL.h>

#include "streamingtexture.h"
#include "tgx.h"
#include "gm1.h"
#include "SDLWindow.h"
#include "surface.h"
#include "rw.h"

struct DestroyRendererDeleter
{
    void operator()(SDL_Renderer *renderer) const {
        if(renderer != NULL) {
            SDL_DestroyRenderer(renderer);
        }
    };
};

class Renderer
{
    SDL_Renderer *renderer;

    typedef struct {
        gm1::Collection gm1;
        Surface atlas;
    } CacheEntry;

    std::map<std::string, CacheEntry> m_cache;

    void ReallocateStreamingTexture(int width, int height);
    Surface AllocateStreamingSurface(void *pixels, int width, int height, int pitch);

    Uint32 fbFormat;
    SDL_Texture *fbTexture;

    // Null when texture is unlocked
    Surface fbSurface;
    
public:
    Renderer(SDLWindow &window);
    ~Renderer();

    inline SDL_Renderer *GetRenderer() { return renderer; }
    
    Surface BeginFrame();
    void EndFrame();
    
    Surface LoadImage(const std::string &filename);
    bool LoadImageCollection(const std::string &filename);
    
};

void EnumRenderDrivers();
void PrintRendererInfo(SDL_Renderer *renderer);

#endif
