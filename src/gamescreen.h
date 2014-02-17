#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>

#include <SDL2/SDL.h>

#include "screen.h"
#include "renderer.h"

class GameScreen : public Screen
{
public:
    GameScreen();
    ~GameScreen();
    
    void OnFrame(Renderer &renderer);
    void OnEvent(const SDL_Event &event);
    void OnEnterEventLoop();

    bool Closed() const;
    
    std::unique_ptr<Screen> NextScreen();
};

#endif
