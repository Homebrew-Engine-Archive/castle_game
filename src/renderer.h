#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdexcept>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <SDL2/SDL.h>

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
    SDL_Renderer *rndr;

    std::map<
        std::string,
        std::vector<SDL_Rect>> atlasPartition;

    std::map<
        std::string,
        SDL_Palette*> atlasPalettes;
    
    std::map<
        std::string,
        Surface> atlasStorage;
    
    std::map<
        std::string,
        Surface> imageStorage;
    
public:
    Renderer(SDLWindow &window);
    ~Renderer();

    SDL_Rect OutputRect() const;
    
    void BeginFrame();
    void EndFrame();
    void Clear();
    void Present();
    void FillRect(const SDL_Rect *rect);
    
    void BlitImage(const std::string &name, const SDL_Rect *srcrect, const SDL_Rect *dstrect);
    void BlitCollectionImage(const std::string &filename, size_t index);
    
    bool LoadImage(const std::string &filename);
    bool LoadImageCollection(const std::string &filename);
};

#endif
