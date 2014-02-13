#ifndef GAME_H_
#define GAME_H_

#include <memory>

#include <SDL2/SDL.h>

#include "gamescreen.h"
#include "screen.h"
#include "SDLRenderer.h"

class Game
{
    bool m_Closed;
    Uint32 m_CurrentMouseInvalid;
    Uint32 m_CurrentMouseX;
    Uint32 m_CurrentMouseY;
    std::unique_ptr<Screen> m_ActiveScreen;
    
public:
    Game();
    ~Game();

    void OnTimeElapsed(Uint32 ms);
    void OnFrame(SDLRenderer &renderer);
    void OnKeyDown(const SDL_KeyboardEvent &event);
    void OnKeyUp(const SDL_KeyboardEvent &event);
    void OnMouseMotion(const SDL_MouseMotionEvent &event);
    void OnMouseButtonDown(const SDL_MouseButtonEvent &event);
    void OnMouseButtonUp(const SDL_MouseButtonEvent &event);
    void OnQuit();

    bool IsClosed() const;
};

#endif
