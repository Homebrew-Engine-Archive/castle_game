#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <vector>
#include <iostream>
#include <functional>
#include "filesystem.h"
#include "surface.h"

namespace Castle
{
    class Engine;
}

namespace Render
{
    class Renderer;
}

class FontCollectionInfo;

class LoadingScreen
{
    Render::Renderer *mRenderer;
    Castle::Engine *mEngine;
    Surface mBackground;
    bool mQuit;
    std::vector<std::function<void()>> mTasks;

    void ScheduleCacheGM1(const FilePath &filename);
    void ScheduleCacheFont(const FontCollectionInfo &info);
    
public:
    LoadingScreen(Castle::Engine *engine);
    bool Exec();
    void Draw(double done);
};

bool RunLoadingScreen(Castle::Engine *engine);

#endif
