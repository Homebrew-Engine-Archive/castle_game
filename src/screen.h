#ifndef SCREEN_H_
#define SCREEN_H_

#include <memory>
#include <SDL2/SDL.h>

#include "renderer.h"

inline int PeekEvent(SDL_Event *e)
{
    return SDL_PeepEvents(e, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

class Screen
{
public:
    virtual void Draw(Surface frame) = 0;
    virtual bool HandleEvent(const SDL_Event &event) = 0;
};

#endif
