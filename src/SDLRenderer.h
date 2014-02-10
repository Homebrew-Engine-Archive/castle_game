#ifndef SDLRENDERER_H_
#define SDLRENDERER_H_

#include <SDL2/SDL.h>

#include "errors.h"
#include "SDLWindow.h"

class SDLRenderer
{
    SDL_Renderer *m_R;
public:
    SDLRenderer(SDLWindow &window);
    ~SDLRenderer();

    void Present();
};

#endif
