#ifndef SCREEN_H_
#define SCREEN_H_

#include <memory>
#include <SDL2/SDL.h>

#include "renderer.h"

class Screen
{
public:
    virtual void Draw(Renderer &renderer) = 0;
    virtual bool HandleEvent(const SDL_Event &event) = 0;
};

#endif
