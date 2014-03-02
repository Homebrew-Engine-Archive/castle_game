#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <set>

#include <SDL2/SDL.h>

#include "events.h"
#include "gamescreen.h"
#include "screen.h"
#include "gm1.h"
#include "renderer.h"
#include "textrenderer.h"
#include "font.h"

class LoadingScreen
{
    Renderer &m_renderer;
    Surface m_background;
    bool m_quit;
    std::vector<std::string> m_files;

    void Draw(double done);
    void HandleEvents();
    
public:
    LoadingScreen(Renderer &renderer);
    int Exec();
};

int RunLoadingScreen(Renderer &renderer);

#endif
