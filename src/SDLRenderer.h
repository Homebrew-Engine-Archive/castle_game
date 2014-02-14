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

struct DrawingPlain
{
    std::shared_ptr<SDLSurface> surface;
    std::vector<SDL_Rect> rects;
};

class SDLRenderer
{
    SDL_Renderer *rndr;

    std::map<
        std::string,
        DrawingPlain> resources;

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
        std::shared_ptr<SDLSurface>,
        const std::vector<SDL_Rect> &rects);
    
    void CopyDrawingPlain(
        const std::string &name,
        const SDL_Rect *srcrect,
        SDL_Rect *dstrect = NULL);

    SDL_Rect QuerySurfaceRect(
        const std::string &name,
        size_t index);
    
    void ResizeOutput(int width, int height);
};

#endif
