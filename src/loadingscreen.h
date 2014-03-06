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
    Renderer *m_renderer;
    RootScreen *m_root;
    bool m_quit;
    std::vector<std::string> m_files;

public:
    LoadingScreen(RootScreen *root);
    int Exec();
    void Draw(double done);
};

int RunLoadingScreen(RootScreen *root);

#endif
