#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <functional>
#include <iostream>
#include <fstream>
#include <memory>
#include <set>
#include "SDL.h"

#include "events.h"
#include "gamescreen.h"
#include "screen.h"
#include "gm1.h"
#include "renderer.h"
#include "textrenderer.h"
#include "font.h"

class LoadingScreen
{
    Renderer *m_renderer;
    RootScreen *m_root;
    bool m_quit;
    std::vector<std::function<void()>> m_tasks;

    void ScheduleCacheGM1(const std::string &filename);
    void ScheduleCacheFont(const FontAtlasInfo &info);
    
public:
    LoadingScreen(RootScreen *root);
    int Exec();
    void Draw(double done);
};

int RunLoadingScreen(RootScreen *root);

#endif
