#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>

#include <SDL2/SDL.h>

#include "screen.h"
#include "SDLRenderer.h"

class GameScreen : public Screen
{
public:
    void Draw(SDLRenderer &renderer);
    void OnMouseMotion(const SDL_MouseMotionEvent &event);
    void OnMouseButtonDown(const SDL_MouseButtonEvent &event);
    void OnMouseButtonUp(const SDL_MouseButtonEvent &event);
    void OnKeyDown(const SDL_KeyboardEvent &event);
    void OnKeyUp(const SDL_KeyboardEvent &event);

    std::unique_ptr<Screen> NextScreen();
};

#endif
