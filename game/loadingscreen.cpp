#include "loadingscreen.h"

#include <iostream>

#include <SDL.h>

#include <boost/filesystem/fstream.hpp>

#include <game/clamp.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/filesystem.h>
#include <game/renderer.h>
#include <game/collection.h>

namespace UI
{
    LoadingScreen::LoadingScreen()
        : mBackground(
            Castle::LoadTGX(
                fs::TGXFilePath("frontend_loading")))
        , mProgressDone(0)
        , mProgressMax(1)
    {
    }

    bool LoadingScreen::HandleEvent(SDL_Event const&)
    {
        return false;
    }

    void LoadingScreen::IncreaseDone(int delta)
    {
        mProgressDone += delta;
    }
    
    void LoadingScreen::SetProgressDone(int done)
    {
        mProgressDone = done;
    }

    void LoadingScreen::SetProgressMax(int max)
    {
        mProgressMax = max;
    }

    void LoadingScreen::SetProgressLabel(const std::string &text)
    {
        mStage = text;
    }
    
    double LoadingScreen::GetCompleteRate() const
    {
        double done = static_cast<double>(mProgressDone) / mProgressMax;
        return core::Clamp(done, 0.0f, 1.0f);
    }
    
    void LoadingScreen::Render(Render::Renderer &renderer)
    {
    }   
} // namespace UI
