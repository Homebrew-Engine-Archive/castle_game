#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <memory>
#include <set>

#include <SDL2/SDL.h>

#include "events.h"
#include "gamescreen.h"
#include "game.h"
#include "errors.h"
#include "screen.h"
#include "gm1.h"
#include "renderer.h"
#include "textrenderer.h"
#include "font.h"

class LoadingScreen : public Screen
{
    Renderer &renderer;
    bool m_closed;
    std::set<std::string> m_files;
    std::unique_ptr<Screen> m_nextscr;
    size_t m_total;
    size_t m_completed;

    double GetPercentageComplete() const;

    void SetPreloadsList(const std::string &filename);

public:
    LoadingScreen(Renderer &renderer);
    ~LoadingScreen();

    void OnEnterEventLoop();
    void OnFrame(Renderer &renderer);
    void OnEvent(const SDL_Event &event);

    bool Closed() const;
    
    std::unique_ptr<Screen> NextScreen();
};

#endif
