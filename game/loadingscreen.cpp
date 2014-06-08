#include "loadingscreen.h"

#include <iostream>

#include <SDL.h>

#include <boost/filesystem/fstream.hpp>

#include <game/clamp.h>

#include <game/color.h>
#include <game/rect.h>
#include <game/filesystem.h>
#include <game/collection.h>
#include <game/surface_drawing.h>

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
    
    void LoadingScreen::Draw(Surface &frame)
    {
        double rate = GetCompleteRate();
        
        Rect frameRect(frame);
        Rect bgRect(mBackground);

        Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);
        BlitSurface(mBackground, bgRect, frame, bgAligned);
    
        Rect barOuter(300, 25);
        Rect barOuterAligned = PutIn(barOuter, bgAligned, 0, 0.8f);
        Graphics::FillFrame(frame, barOuterAligned, Colors::Black.Opaque(100));
        Graphics::DrawFrame(frame, barOuterAligned, Colors::Black);

        Rect barOuterPadded = PadIn(barOuterAligned, 5);
        Rect barInner(barOuterPadded.w * rate, barOuterPadded.h);
        Rect barInnerAligned = PutIn(barInner, barOuterPadded, -1.0f, 0);
        Graphics::FillFrame(frame, barInnerAligned, Colors::Black);
    }   
} // namespace UI
