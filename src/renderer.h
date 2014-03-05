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
#include "window.h"
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

struct CollectionEntry
{
    gm1::ImageHeader header;
    Surface surface;
    CollectionEntry(const gm1::ImageHeader &hdr_, const Surface &sf_)
        : header(hdr_)
        , surface(sf_)
        {}
};

struct CollectionData
{
    gm1::Header header;
    std::vector<CollectionEntry> entries;
    std::vector<SDL_Palette *> palettes;
};

class Renderer
{
    SDL_Renderer *renderer;
    SDL_Texture *fbTexture;
    Surface fbSurface;
    int fbWidth;
    int fbHeight;
    Uint32 fbFormat;
    std::map<std::string, CollectionData> m_cache;
    std::map<std::string, Surface> m_imageCache;

    bool AllocFrameTexture(int width, int height);
    Surface AllocFrameSurface(void *pixels, int width, int height, int pitch);
    
public:
    Renderer(Window &window);
    ~Renderer();

    inline SDL_Renderer *GetRenderer() { return renderer; }
    
    Surface BeginFrame();
    void EndFrame();

    SDL_Rect GetOutputSize() const;
    void AdjustOutputSize(int width, int height);
    
    Surface QuerySurface(const std::string &filename);
    
    Surface LoadSurface(const std::string &filename);
    bool LoadImageCollection(const std::string &filename);

    CollectionData GetCollection(const std::string &filename) const;
};

void EnumRenderDrivers();
void PrintRendererInfo(SDL_Renderer *renderer);

#endif
