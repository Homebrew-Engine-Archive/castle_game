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

namespace UI
{
    class LoadingScreen : public UI::Screen
    {
        Castle::Engine *mEngine;
        Surface mBackground;
        int mProgressDone;
        int mProgressMax;
        std::string mStage;
        bool mDirty;

    public:
        LoadingScreen(Castle::Engine*);
        void SetProgressDone(int done);
        void SetProgressMax(int max);
        void IncreaseDone(int delta = 1);
        void SetProgressLabel(std::string const&);
        double GetCompleteRate() const;
        bool IsDirty(int64_t elapsed) const;
        bool Exec();
        void Draw(Surface &surface);
        bool HandleEvent(SDL_Event const&);
    };
}

#endif  // LOADINGSCREEN_H_
