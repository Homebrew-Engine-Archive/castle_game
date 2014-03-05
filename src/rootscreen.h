#ifndef ROOTSCREEN_H_
#define ROOTSCREEN_H_

#include <fstream>
#include <SDL2/SDL.h>
#include <boost/asio/io_service.hpp>

class RootScreen;

#include "network.h"
#include "persist_value.h"
#include "screen.h"
#include "gamescreen.h"
#include "loadingscreen.h"
#include "main_menu_screen.h"
#include "macrosota.h"

class RootScreen
{
    bool m_closed;
    int m_frameRate;
    bool m_frameLimit;
    Renderer &m_renderer;
    std::unique_ptr<Screen> m_currentScreen;
    
    bool HandleWindowEvent(const SDL_WindowEvent &window);
    bool HandleKeyboardEvent(const SDL_KeyboardEvent &keyboard);
    
public:
    RootScreen(Renderer &renderer);

    int Exec();
    
    bool HandleEvent(const SDL_Event &event);

    void SetCurrentScreen(std::unique_ptr<Screen> screen);
    void PushScreen(std::unique_ptr<Screen> screen);

    inline int GetMaximumFPS() const { return m_frameRate; }
    inline bool HasFrameLimit() const { return m_frameLimit; }
    
    inline void SetMaximumFPS(int fps) { m_frameRate = fps; }
    inline void SetFrameLimit(bool limit) { m_frameLimit = limit; }
};

#endif
