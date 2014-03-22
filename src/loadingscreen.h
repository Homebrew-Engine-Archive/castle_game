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

class Renderer;
class FontCollectionInfo;

class LoadingScreen
{
    Renderer *mRenderer;
    Castle::Engine *mRoot;
    Surface mBackground;
    bool mQuit;
    std::vector<std::function<void()>> mTasks;

    void ScheduleCacheGM1(const FilePath &filename);
    void ScheduleCacheFont(const FontCollectionInfo &info);
    
public:
    LoadingScreen(Castle::Engine *root);
    bool Exec();
    void Draw(double done);
};

bool RunLoadingScreen(Castle::Engine *root);

#endif
