#ifndef SDLRENDERER_H_
#define SDLRENDERER_H_

#include <SDL2/SDL.h>

#include "errors.h"
#include "SDLWindow.h"
#include "SDLSurface.h"

class SDLRenderer
{
    SDL_Renderer *m_R;
public:
    SDLRenderer(SDLWindow &window);
    ~SDLRenderer();

    SDL_Renderer* GetRenderer();
    void Present();

    SDL_Texture* CreateTextureFromSurface(SDLSurface &surface);
};

#endif
