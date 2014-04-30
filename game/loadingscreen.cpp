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
#include <game/engine.h>

namespace UI
{
    LoadingScreen::LoadingScreen(Castle::Engine *engine)
        : mEngine(engine)
        , mBackground(nullptr)
        , mProgressDone(0)
        , mProgressMax(1)
        , mDirty(false)
    {
        const fs::path filepath = fs::TGXFilePath("frontend_loading");
        mBackground = LoadSurface(filepath);
    }
    
    bool LoadingScreen::Exec()
    {
        return true;
    }

    bool LoadingScreen::HandleEvent(SDL_Event const&)
    {
        return true;
    }

    bool LoadingScreen::IsDirty(int64_t elapsed) const
    {
        return true;
    }
    
    void LoadingScreen::SetProgressDone(int done)
    {
    }

    void LoadingScreen::SetProgressMax(int max)
    {
    }

    void LoadingScreen::SetProgressLabel(std::string const& text)
    {
    }
    
    double LoadingScreen::GetCompleteRate() const
    {
        double done = static_cast<double>(mProgressDone) / mProgressMax;
        return boost::algorithm::clamp(done, 0.0f, 1.0f);
    }
    
    void LoadingScreen::Draw(Surface &frame)
    {
        mDirty = false;
        double rate = GetCompleteRate();
        
        SDL_Rect frameRect = SurfaceBounds(frame);
        SDL_Rect bgRect = SurfaceBounds(mBackground);

        SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);
        BlitSurface(mBackground, NULL, frame, &bgAligned);
    
        SDL_Rect barOuter = MakeRect(300, 25);
        SDL_Rect barOuterAligned = PutIn(barOuter, bgAligned, 0, 0.8f);
        FillFrame(frame, &barOuterAligned, 0x7f000000);
        DrawFrame(frame, &barOuterAligned, 0xff000000);

        SDL_Rect barOuterPadded = PadIn(barOuterAligned, 5);
        SDL_Rect barInner = MakeRect(barOuterPadded.w * rate, barOuterPadded.h);
        SDL_Rect barInnerAligned = PutIn(barInner, barOuterPadded, -1.0f, 0);
        FillFrame(frame, &barInnerAligned, 0xff000000);
    }   
} // namespace UI
