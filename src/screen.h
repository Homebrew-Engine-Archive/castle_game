#ifndef SCREEN_H_
#define SCREEN_H_

#include <memory>
#include <SDL2/SDL.h>

#include "renderer.h"

class Screen
{
public:
    virtual void Draw(Renderer &renderer) = 0;
    virtual void OnMouseMotion(const SDL_MouseMotionEvent &event) = 0;
    virtual void OnMouseButtonDown(const SDL_MouseButtonEvent &event) = 0;
    virtual void OnMouseButtonUp(const SDL_MouseButtonEvent &event) = 0;
    virtual void OnKeyDown(const SDL_KeyboardEvent &event) = 0;
    virtual void OnKeyUp(const SDL_KeyboardEvent &event) = 0;

    virtual std::unique_ptr<Screen> NextScreen() = 0;
};

#endif
