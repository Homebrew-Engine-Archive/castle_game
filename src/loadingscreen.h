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

namespace UI
{

    class Screen;
    
    class LoadingScreen
    {
        Render::Renderer *mRenderer;
        Castle::Engine *mEngine;
        Surface mBackground;
        std::vector<std::function<void()>> mTasks;

        void ScheduleCacheGM1(const FilePath &filename);
        void ScheduleCacheFont(const FontCollectionInfo &info);
    
    public:
        LoadingScreen(Castle::Engine *engine);

        void SetDonePercentage(double done);
        
        bool Exec();
        void Draw(double done);
    };

    bool RunLoadingScreen(Castle::Engine *engine);
    
}

#endif
