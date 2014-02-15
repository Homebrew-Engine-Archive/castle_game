#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>
#include <vector>
#include <string>

#include <SDL2/SDL.h>

#include "gm1.h"
#include "errors.h"
#include "SDLWindow.h"
#include "surface.h"

struct Atlas
{
    SDL_Texture *texture;
    std::shared_ptr<Surface> surface;
    std::vector<SDL_Rect> rects;
    const SDL_Palette *palettes;
    size_t paletteCount;
    Uint32 anchorX;
    Uint32 anchorY;
};

class Renderer
{
    SDL_Renderer *rndr;

    std::vector<std::shared_ptr<Surface>> atlases;
    
public:
    Renderer(SDLWindow &window);
    ~Renderer();

    void BeginFrame();
    void EndFrame();

    void Present();
    
    void LoadGM1Atlas(SDL_RWops *src, const gm1::Collection &gm1);
};

#endif
