#ifndef SCREEN_H_
#define SCREEN_H_

#include <memory>
#include <SDL2/SDL.h>

#include "SDLRenderer.h"

class Screen
{
public:
    virtual void Draw(const SDLRenderer &renderer, const SDL_Rect &dstrect) = 0;
    virtual void OnMouseMotion(const SDL_MouseMotionEvent &event) = 0;
    virtual void OnMouseButton(const SDL_MouseButtonEvent &event) = 0;
    virtual void OnKeyboard(const SDL_Keyboard &event) = 0;

    virtual void EnterEventLoop() = 0;
    virtual std::unique_ptr<Screen> NextScreen() = 0;
};

#endif
