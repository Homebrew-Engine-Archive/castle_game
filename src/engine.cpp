#include "engine.h"

#include <sstream>
#include <string>
#include <boost/asio/io_service.hpp>
#include <vector>
#include <SDL.h>

#include "geometry.h"
#include "screenmanager.h"
#include "renderer.h"
#include "menu_combat.h"
#include "menu_main.h"
#include "loadingscreen.h"
#include "macrosota.h"
#include "network.h"
#include "debugconsole.h"
#include "screen.h"

namespace Castle
{
    
    Engine::Engine(Render::Renderer *renderer)
        : mRenderer(renderer)
        , mFpsAverage(0.0f)
        , mFrameCounter(0)
        , mClosed(false)
        , mFrameRate(16)
        , mFpsLimited(true)
        , mShowConsole(false)
        , mConsolePtr(nullptr)
        , mConsoleInput()
        , mConsoleOutput()
        , mScreenMgr(new UI::ScreenManager(this))
    { }

    bool Engine::HandleWindowEvent(const SDL_WindowEvent &window)
    {
        switch(window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            {
                int width = window.data1;
                int height = window.data2;
                mRenderer->AdjustBufferSize(width, height);
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
            ToggleConsole();
            return true;
        default:
            return true;
        }
    }

    void Engine::ToggleConsole()
    {
        mShowConsole = !mShowConsole;
        if(mShowConsole) {
            UI::ScreenPtr &&consoleScreen = UI::CreateDebugConsole(mScreenMgr.get(), mRenderer, mConsoleInput, mConsoleOutput);
            mConsolePtr = consoleScreen.get();
            mScreenMgr->PushScreen(std::move(consoleScreen));
        } else {
            mScreenMgr->CloseScreen(mConsolePtr);
            mConsolePtr = nullptr;
        }
    }
    
    bool Engine::Closed() const
    {
        return mClosed;
    }
    
    UI::Screen *Engine::GetCurrentScreen() const
    {
        return mScreenMgr->GetCurrentScreen();
    }

    void Engine::SetCurrentScreen(UI::ScreenPtr &&screen)
    {
        mScreenMgr->SetCurrentScreen(std::move(screen));
    }

    void Engine::PollInput()
    {
        if(!mConsoleInput) {
            std::string line;
            std::getline(mConsoleInput, line);
            std::cout << line << std::endl;
        }
        
        SDL_Event event;
        if(GetCurrentScreen() != NULL) {
            while(SDL_PollEvent(&event)) {
                if(!GetCurrentScreen()->HandleEvent(event))
                    HandleEvent(event);
            }
        } else {
            while(SDL_PollEvent(&event))
                HandleEvent(event);
        }
    }
    
    int Engine::Exec()
    {
        if(!UI::RunLoadingScreen(this)) {
            std::clog << "Loading has been interrupted."
                      << std::endl;
            return false;
        }

        mScreenMgr->PushScreen(UI::CreateMenuMain(mScreenMgr.get(), mRenderer));

        const int64_t msPerSec = 1000;
        const int64_t pollRate = 66;
        const int64_t pollInterval = msPerSec / pollRate;
    
        int64_t lastFrame = 0;
        int64_t lastPoll = 0;
        int64_t lastSecond = 0;
        int64_t fpsCounterLastSecond = 0;
    
        boost::asio::io_service io;

        const short port = 4500;
        Server server(io, port);
        server.StartAccept();
    
        while(!Closed()) {
            PollInput();
            
            const int64_t frameInterval = msPerSec / mFrameRate;
            const int64_t pollStart = SDL_GetTicks();
            if(lastPoll + pollInterval < pollStart) {
                io.poll();
                lastPoll = pollStart;
            }

            const int64_t frameStart = SDL_GetTicks();
            if(lastSecond + msPerSec < frameStart) {
                double fps = mFrameCounter - fpsCounterLastSecond;
                double elapsed = frameStart - lastSecond;
                mFpsAverage = fps * (msPerSec / elapsed);
                lastSecond = frameStart;
                fpsCounterLastSecond = mFrameCounter;
            }

            if(lastFrame + frameInterval < frameStart) {
                DrawFrame();
                lastFrame = frameStart;
                ++mFrameCounter;
            }

            const int64_t delayStart = SDL_GetTicks();
            if(mFpsLimited) {
                const int64_t nextTick =
                    std::min(lastPoll + pollInterval,
                             lastFrame + frameInterval);
                if(delayStart < nextTick) {
                    SDL_Delay(nextTick - delayStart);
                }
            }
        }

        std::clog << "Gracefully shutdown" << std::endl;
    
        return 0;
    }

    void Engine::DrawFrame()
    {
        Surface frame = mRenderer->BeginFrame();

        mScreenMgr->DrawScreen(frame);
    
        SDL_Color color = MakeColor(255, 255, 255, 128);
        mRenderer->SetColor(color);
        mRenderer->SetFont("font_stronghold_aa", 24);

        SDL_Point pos = ShiftPoint(TopLeft(mRenderer->GetOutputSize()), 5, 5);
    
        std::ostringstream oss;
        oss << "FPS: " << mFpsAverage;
        mRenderer->RenderTextLine(oss.str(), pos);

        mRenderer->EndFrame();
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

    Render::Renderer *Engine::GetRenderer()
    {
        return mRenderer;
    }

}
