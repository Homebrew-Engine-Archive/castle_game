#ifndef ROOTSCREEN_H_
#define ROOTSCREEN_H_

class RootScreen;

#include <iostream>
#include <sstream>
#include <fstream>
#include "SDL.h"

#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>

#include "network.h"
#include "persist_value.h"
#include "screen.h"
#include "gamescreen.h"
#include "loadingscreen.h"
#include "menu_main.h"
#include "macrosota.h"

class RootScreen : public boost::noncopyable
{
private:
    Renderer *m_renderer;
    double m_fpsAverage;
    bool m_closed;
    int m_frameRate;
    bool m_frameLimit;
    std::unique_ptr<Screen> m_currentScreen;

    bool HandleWindowEvent(const SDL_WindowEvent &window);
    bool HandleKeyboardEvent(const SDL_KeyboardEvent &keyboard);
    
public:
    RootScreen(Renderer *renderer);

    int Exec();

    void DrawFrame();

    bool HandleEvent(const SDL_Event &event);

    void SetCurrentScreen(std::unique_ptr<Screen> screen);
    void PushScreen(std::unique_ptr<Screen> screen);

    inline int GetMaximumFPS() const { return m_frameRate; }
    inline bool HasFrameLimit() const { return m_frameLimit; }
    
    inline void SetMaximumFPS(int fps) { m_frameRate = fps; }
    inline void SetFrameLimit(bool limit) { m_frameLimit = limit; }

    inline Renderer *GetRenderer() { return m_renderer; }
};

#endif
