#ifndef SDLRENDERER_H_
#define SDLRENDERER_H_

#include <map>
#include <vector>
#include <string>

#include <SDL2/SDL.h>

#include "gm1.h"
#include "errors.h"
#include "SDLWindow.h"
#include "SDLSurface.h"

class SDLRenderer
{
    SDL_Renderer *rndr;

    std::map<
        std::string,
        std::shared_ptr<SDLSurface>> surfaces;

    std::map<
        std::pair<
            std::string,
            size_t>,
        SDL_Rect> layouts;

    SDL_Surface *frameBuffer;
    bool bfBufferInProgress;

    void InitialSetSize();
    
public:
    SDLRenderer(SDLWindow &window);
    ~SDLRenderer();

    void BeginFrame();
    void EndFrame();
    
    SDL_Renderer* GetRenderer();
    void Present();

    void RegisterDrawingPlain(
        const std::string &name,
        std::shared_ptr<SDLSurface>);
    
    void CopyDrawingPlain(
        const std::string &name,
        const SDL_Rect *srcrect,
        SDL_Rect *dstrect = NULL);

    void RegisterSurfaceRect(
        const std::string &name,
        size_t index,
        const SDL_Rect &rect);    

    SDL_Rect QuerySurfaceRect(
        const std::string &name,
        size_t index);
    
    void ResizeOutput(int width, int height);
};

#endif
