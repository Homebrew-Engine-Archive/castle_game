#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <iostream>
#include <functional>
#include "rootscreen.h"
#include "renderer.h"
#include "surface.h"

class LoadingScreen
{
    Renderer *m_renderer;
    RootScreen *m_root;
    Surface m_background;
    bool m_quit;
    std::vector<std::function<void()>> m_tasks;

    void ScheduleCacheGM1(const FilePath &filename);
    void ScheduleCacheFont(const FontCollectionInfo &info);
    
public:
    LoadingScreen(RootScreen *root);
    int Exec();
    void Draw(double done);
};

int RunLoadingScreen(RootScreen *root);

#endif
