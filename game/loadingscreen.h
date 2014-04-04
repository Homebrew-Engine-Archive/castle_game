#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <vector>
#include <iosfwd>
#include <functional>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/screen.h>

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

    class LoadingScreen : public UI::Screen
    {
        Render::Renderer *mRenderer;
        Castle::Engine *mEngine;
        Surface mBackground;
        std::vector<std::function<void()>> mTasks;
        int mProgressDone;
        int mProgressMax;
        bool mDirty;

        void ScheduleCacheGM1(FilePath const&);
        void ScheduleCacheFont(FontCollectionInfo const&);

    public:
        LoadingScreen(Castle::Engine*);
        void SetProgressDone(int done);
        void SetProgressMax(int max);
        void SetProgressLabel(std::string const&);
        double GetCompleteRate() const;
        bool IsDirty(int64_t elapsed) const;
        bool Exec();
        void Draw(Surface &surface);
        bool HandleEvent(SDL_Event const&);
    };

    std::unique_ptr<LoadingScreen> CreateLoadingScreen(Castle::Engine *engine);
    
}

#endif
