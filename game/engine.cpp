#include "engine.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <thread>

#include <SDL.h>

#include <game/textrenderer.h>
#include <game/gamescreen.h>
#include <game/renderer.h>
#include <game/menu_combat.h>
#include <game/menu_main.h>
#include <game/loadingscreen.h>
#include <game/screen.h>

namespace Castle
{
    Engine::Engine(Render::Renderer &renderer)
        : mRenderer(renderer)
        , mFpsAverage(0.0f)
        , mFpsAverageMax(mFpsAverage)
        , mFrameCounter(0)
        , mClosed(false)
        , mFrameUpdateInterval(std::chrono::milliseconds(0))
        , mFpsUpdateInterval(std::chrono::seconds(3))
        , mFpsLimited(false)
        , mShowConsole(false)
        , mIO()
        , mPort(4500)
        , mFontMgr()
        , mSimulationMgr()
        , mScreenMgr(mRenderer, mFontMgr, mSimulationMgr)
        , mServer(mIO, mPort)
    { }

    bool Engine::HandleWindowEvent(const SDL_WindowEvent &window)
    {
        switch(window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            {
                const int width = window.data1;
                const int height = window.data2;
                mRenderer.SetWindowSize(width, height);
            }
            return true;
        default:
            return true;
        }
    }

    bool Engine::HandleKeyboardEvent(const SDL_KeyboardEvent &key)
    {
        switch(key.keysym.sym) {
        case SDLK_ESCAPE:
            mClosed = true;
            return false;
        case SDLK_BACKSLASH:
            return true;
        default:
            return true;
        }
    }

    bool Engine::HandleEvent(const SDL_Event &event)
    {
        switch(event.type) {
        case SDL_WINDOWEVENT:
            return HandleWindowEvent(event.window);
        case SDL_QUIT:
            mClosed = true;
            return false;
        case SDL_KEYDOWN:
            return HandleKeyboardEvent(event.key);
        default:
            return true;
        }
    }
    
    void Engine::LoadFonts()
    {
        const int sizes[] = {8, 9, 11, 13, 15, 17, 19, 23, 30, 45};
        const std::string fontset = Render::FontStronghold;
        for(int fsize : sizes) {
            mFontMgr.LoadFontFile(fontset, fsize);
        }

        mFontMgr.SetDefaultFont(Render::FontStronghold, 13);
    }

    void Engine::PollInput()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!mScreenMgr.TopScreen()->HandleEvent(event)) {
                HandleEvent(event);
            }
        }
    }
    
    void Engine::DrawFrame()
    {
        Surface frame = mRenderer.BeginFrame();
        mScreenMgr.DrawScreen(frame);
        
        std::ostringstream oss;
        oss << "(Castle game project) " << "FPS: ";
        oss.width(10);
        oss << mFpsAverage << ' ';
        oss << "Max FPS: ";
        oss.width(10);
        oss << mFpsAverageMax << ' ';
        std::string text = oss.str();

        Render::TextRenderer textRenderer(frame);
        textRenderer.SetFont(mFontMgr.DefaultFont());
        textRenderer.SetClipBox(MakeRect(0, 0, 100, 100));
        textRenderer.SetFontStyle(Render::FontStyle_Bold | Render::FontStyle_Italic);
        textRenderer.SetCursorMode(Render::CursorMode::BaseLine);
        textRenderer.Translate(0, 20);
        textRenderer.SetColor(MakeColor(255, 0, 0, 255));

        FillFrame(frame, textRenderer.CalculateTextRect(text), MakeColor(0, 0, 0, 100));
        textRenderer.PutString(text);

        mRenderer.EndFrame();
    }

    void Engine::UpdateFrameCounter(std::chrono::milliseconds elapsed)
    {
        const std::chrono::milliseconds oneSecond = std::chrono::seconds(1);
        const double preciseFrameCounter = mFrameCounter * oneSecond.count();
        mFpsAverage = preciseFrameCounter / elapsed.count();
        mFpsAverageMax = std::max(mFpsAverageMax, mFpsAverage);
        mFrameCounter = 0;
    }
    
    int Engine::Exec()
    {
        using namespace std::chrono;

        LoadFonts();
        mScreenMgr.EnterGameScreen();
        mServer.StartAccept();
        
        steady_clock::time_point prevSimulation = steady_clock::now();
        steady_clock::time_point prevFrame = steady_clock::now();
        steady_clock::time_point prevSecond = steady_clock::now();

        while(!mClosed) {
            PollInput();
            mIO.poll();
            
            if(!mFpsLimited || prevFrame + mFrameUpdateInterval < steady_clock::now()) {
                mFrameCounter += 1;
                prevFrame = steady_clock::now();
                DrawFrame();
            }

            milliseconds sinceLastSim = duration_cast<milliseconds>(steady_clock::now() - prevSimulation);
            if(mSimulationMgr.HasUpdate(sinceLastSim)) {
                prevSimulation = steady_clock::now();
                mSimulationMgr.Update();
            }
            
            if(prevSecond + mFpsUpdateInterval < steady_clock::now()) {
                const milliseconds elapsed = duration_cast<milliseconds>(steady_clock::now() - prevSecond);
                UpdateFrameCounter(elapsed);
                prevSecond = steady_clock::now();
            }
        }

        return 0;
    }
}
