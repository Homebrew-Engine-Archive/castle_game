#include "loadingscreen.h"

#include <memory>
#include <sstream>
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/clamp.hpp>
#include <SDL.h>

#include <game/make_unique.h>
#include <game/filesystem.h>
#include <game/sdl_utils.h>
#include <game/renderer.h>

namespace UI
{
    LoadingScreen::LoadingScreen(Castle::Engine *engine)
        : mEngine(engine)
        , mBackground(
            LoadSurface(
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
        return boost::algorithm::clamp(done, 0.0f, 1.0f);
    }
    
    void LoadingScreen::Draw(Surface &frame)
    {
        double rate = GetCompleteRate();
        
        SDL_Rect frameRect = SurfaceBounds(frame);
        SDL_Rect bgRect = SurfaceBounds(mBackground);

        SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);
        BlitSurface(mBackground, NULL, frame, &bgAligned);
    
        SDL_Rect barOuter = MakeRect(300, 25);
        SDL_Rect barOuterAligned = PutIn(barOuter, bgAligned, 0, 0.8f);
        FillFrame(frame, barOuterAligned, MakeColor(0, 0, 0, 128));
        DrawFrame(frame, barOuterAligned, MakeColor(0, 0, 0, 255));

        SDL_Rect barOuterPadded = PadIn(barOuterAligned, 5);
        SDL_Rect barInner = MakeRect(barOuterPadded.w * rate, barOuterPadded.h);
        SDL_Rect barInnerAligned = PutIn(barInner, barOuterPadded, -1.0f, 0);
        FillFrame(frame, barInnerAligned, MakeColor(0, 0, 0, 255));
    }   
} // namespace UI
