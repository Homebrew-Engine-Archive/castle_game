#include "engine.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <thread>

#include <SDL.h>

#include <game/make_unique.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/point.h>

#include <game/surface_drawing.h>
#include <game/textrenderer.h>
#include <game/gamescreen.h>
#include <game/renderer.h>
#include <game/menu_combat.h>
#include <game/menu_main.h>
#include <game/loadingscreen.h>
#include <game/screen.h>
#include <game/gamemap.h>

namespace Castle
{
    Engine::Engine()
        : mSDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE)
        , mRenderer()
        , mFpsAverage(0.0f)
        , mFrameCounter(0)
        , mClosed(false)
        , mFrameUpdateInterval(std::chrono::milliseconds(0))
        , mFpsUpdateInterval(std::chrono::seconds(3))
        , mFpsLimited(false)
        , mIO()
        , mPort(4500)
        , mScreenManager()
        , mServer(mIO, mPort)
        , mGraphicsMgr()
    { }

    bool Engine::HandleWindowEvent(const SDL_WindowEvent &window)
    {
        switch(window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            {
                const int width = window.data1;
                const int height = window.data2;
                mRenderer.SetScreenSize(width, height);
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
        case SDLK_q:
            mScreenManager.ToggleConsole();
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
            try {
                Render::LoadFont(fontset, fsize);
            } catch(const std::exception &error) {
                std::cerr << "Load font failed: " << error.what() << std::endl;
            }
        }
    }

    void Engine::LoadGraphics()
    {
        // precache gm dir
    }
    
    void Engine::PollInput()
    {
        try {
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                if(!mScreenManager.TopScreen().HandleEvent(event)) {
                    HandleEvent(event);
                }
            }
        } catch(const std::exception &error) {
            std::cerr << "exception on input handling: " << error.what() << std::endl;
        } catch(...) {
            std::cerr << "unknown exception on input handling" << std::endl;
        }
    }
    
    void Engine::PollNetwork()
    {
        try {
            mIO.poll();
        } catch(const std::exception &error) {
            std::cerr << "exception on polling network: " << error.what() << std::endl;
        } catch(...) {
            std::cerr << "unknown exception on polling network" << std::endl;
        }
        
        // forward connections to simulation manager
        // forward data to simulation manager
    }

    void DrawText(Surface frame, Render::TextRenderer &renderer, const std::string &text, int style, const Color &fg, const Color &bg)
    {
        renderer.SetFontStyle(style);
        renderer.SetColor(fg);
        const Rect textRect = renderer.CalculateTextRect(text);
        Graphics::FillFrame(frame, textRect, bg);
        renderer.PutString(text);
    }
    
    void Engine::DrawFrame()
    {
        Surface frame = mRenderer.BeginFrame();
        //mScreenManager.DrawScreen(frame);
        mScreenManager.Render(mRenderer);

        std::ostringstream oss;
        oss.width(10);
        oss << mFpsAverage;
        
        Render::TextRenderer textRenderer(frame);
        textRenderer.SetCursorPos(Point(0, 20));
        textRenderer.SetFont(Render::FontStronghold, 13);
        DrawText(frame, textRenderer, "(Your jam sir) FPS: ", Render::FontStyle_Italic, Colors::Red, Colors::Black.Opaque(150));
        DrawText(frame, textRenderer, oss.str(), Render::FontStyle_Italic | Render::FontStyle_Underline, Colors::Red, Colors::Black.Opaque(150));

        mRenderer.EndFrame();
    }

    void Engine::UpdateFrameCounter(std::chrono::milliseconds elapsed)
    {
        const std::chrono::milliseconds oneSecond = std::chrono::seconds(1);
        const double preciseFrameCounter = mFrameCounter * oneSecond.count();
        mFpsAverage = preciseFrameCounter / elapsed.count();
        mFrameCounter = 0;
    }

    constexpr std::chrono::milliseconds Elapsed(const std::chrono::steady_clock::time_point &lhs,
                                                const std::chrono::steady_clock::time_point &rhs)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(rhs - lhs);
    }
    
    int Engine::Exec()
    {
        using namespace std::chrono;
        
        LoadFonts();
        LoadGraphics();
        SimulationManager::Instance().SetGameMap(std::make_unique<GameMap>(100));
        GenerateRandomMap(SimulationManager::Instance().GetGameMap());
        mScreenManager.EnterGameScreen();
        mServer.StartAccept();
        
        steady_clock::time_point prevSimulation = steady_clock::now();
        steady_clock::time_point prevFrame = steady_clock::now();
        steady_clock::time_point prevFpsUpdate = steady_clock::now();
        
        while(!mClosed) {
            PollInput();
            PollNetwork();

            {
                const steady_clock::time_point now = steady_clock::now();
                if(!mFpsLimited || prevFrame + mFrameUpdateInterval < now) {
                    mFrameCounter += 1;
                    prevFrame = now;
                    // try {
                        DrawFrame();
                    // } catch(const std::exception &error) {
                    //     std::cerr << "DrawFrame failed: " << error.what() << std::endl;
                    // }
                }
            }

            {
                const steady_clock::time_point now = steady_clock::now();
                milliseconds sinceLastSim = Elapsed(prevSimulation, now);
                if(SimulationManager::Instance().HasUpdate(sinceLastSim)) {
                    prevSimulation = now;
                    SimulationManager::Instance().Update();
                }
            }

            {
                const steady_clock::time_point now = steady_clock::now();
                const milliseconds sinceLastFpsUpdate = Elapsed(prevFpsUpdate, now);
                if(mFpsUpdateInterval < sinceLastFpsUpdate) {
                    prevFpsUpdate = now;
                    UpdateFrameCounter(sinceLastFpsUpdate);
                }
            }
        }

        return 0;
    }
}
