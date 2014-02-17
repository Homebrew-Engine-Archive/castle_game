#ifndef RENDERER_H_
#define RENDERER_H_

#include <set>
#include <map>
#include <vector>
#include <string>

#include <SDL2/SDL.h>

#include "tgx.h"
#include "gm1.h"
#include "errors.h"
#include "SDLWindow.h"
#include "surface.h"
#include "rw.h"

struct Atlas
{
    SDL_Texture *texture;
    Surface surface;
    std::vector<SDL_Rect> rects;
    std::vector<gm1::ImageHeader> headers;
    const SDL_Palette *palettes;
    Uint32 imageCount;
    size_t paletteCount;
    Uint32 anchorX;
    Uint32 anchorY;
    Uint32 colorKey;
};

class Renderer
{
    SDL_Renderer *rndr;

    std::map<
        std::string,
        Surface> atlasStorage;
    
    std::map<
        std::string,
        Surface> imageStorage;

public:
    Renderer(SDLWindow &window);
    ~Renderer();

    void BeginFrame();
    void EndFrame();
    void Clear();
    void Present();
    void FillRect(const SDL_Rect *rect);
    
    void BlitImage(const std::string &name, const SDL_Rect *srcrect, const SDL_Rect *dstrect);
    
    int LoadImage(const std::string &filename);
    int LoadImageCollection(const std::string &filename);
};

#endif
