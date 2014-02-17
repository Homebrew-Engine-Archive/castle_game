#ifndef SCREEN_H_
#define SCREEN_H_

#include <memory>
#include <SDL2/SDL.h>

#include "renderer.h"

class Screen
{
public:
    virtual void OnEnterEventLoop() = 0;
    virtual void OnFrame(Renderer &renderer) = 0;
    virtual void OnEvent(const SDL_Event &event) = 0;
    virtual std::unique_ptr<Screen> NextScreen() = 0;
    virtual bool Closed() const = 0;
};

#endif
