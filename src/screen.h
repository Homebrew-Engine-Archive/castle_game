#ifndef SCREEN_INCLUDED_H_
#define SCREEN_INCLUDED_H_

#include "surface.h"
#include "SDL.h"

struct Screen
{
    virtual void Draw(Surface frame) = 0;
    virtual bool HandleEvent(const SDL_Event &event) = 0;
};

#endif
