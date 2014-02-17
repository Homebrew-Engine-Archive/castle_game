#ifndef GAME_H_
#define GAME_H_

#include <fstream>
#include <memory>

#include <SDL2/SDL.h>

#include "loadingscreen.h"
#include "gamescreen.h"
#include "screen.h"
#include "renderer.h"

class Game
{
private:
    bool m_closed;
    std::unique_ptr<Screen> m_frontscreen;
    void OnKeyDown(const SDL_KeyboardEvent &event);
    void OnKeyUp(const SDL_KeyboardEvent &event);
    void OnMouseMotion(const SDL_MouseMotionEvent &event);
    void OnMouseButtonDown(const SDL_MouseButtonEvent &event);
    void OnMouseButtonUp(const SDL_MouseButtonEvent &event);
    
public:
    Game();
    ~Game();

    void OnFrame(Renderer &renderer);
    void OnEvent(const SDL_Event &event);

    bool IsClosed() const;
};

#endif
