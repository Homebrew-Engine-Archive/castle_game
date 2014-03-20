#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <vector>
#include <iostream>
#include <functional>
#include "filesystem.h"
#include "surface.h"

class Renderer;
class RootScreen;
class FontCollectionInfo;

class LoadingScreen
{
    Renderer *mRenderer;
    RootScreen *mRoot;
    Surface mBackground;
    bool mQuit;
    std::vector<std::function<void()>> mTasks;

    void ScheduleCacheGM1(const FilePath &filename);
    void ScheduleCacheFont(const FontCollectionInfo &info);
    
public:
    LoadingScreen(RootScreen *root);
    int Exec();
    void Draw(double done);
};

int RunLoadingScreen(RootScreen *root);

#endif
