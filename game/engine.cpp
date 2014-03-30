#include "engine.h"

#include <iterator>
#include <sstream>
#include <string>
#include <boost/asio/io_service.hpp>
#include <vector>
#include <SDL.h>

#include "font.h"
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
#include "graphicsmanager.h"
#include "entityclass.h"

namespace Castle
{
    
    Engine::Engine(Render::Renderer *renderer)
        : mRenderer(renderer)
        , mFpsAverage(0.0f)
        , mFrameCounter(0)
        , mClosed(false)
        , mFrameRate(30)
        , mFpsLimited(false)
        , mShowConsole(false)
        , mPollRate(66)
        , mConsolePtr(nullptr)
        , mScreenMgr(new UI::ScreenManager)
        , mIO()
        , mPort(4500)
        , mServer(new Network::Server(mIO, mPort))
        , mGraphicsMgr(new Render::GraphicsManager(mRenderer))
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
            UI::ScreenPtr &&consoleScreen = UI::CreateDebugConsole(mScreenMgr.get(), mRenderer);
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
    
    void Engine::PollInput()
    {
        SDL_Event event;
        if(mScreenMgr->GetCurrentScreen() != NULL) {
            while(SDL_PollEvent(&event)) {
                if(!mScreenMgr->GetCurrentScreen()->HandleEvent(event))
                    HandleEvent(event);
            }
        } else {
            while(SDL_PollEvent(&event))
                HandleEvent(event);
        }
    }

    int Engine::Exec()
    {
        if(!LoadGraphics()) {
            std::cerr << "Loading has been interrupted." << std::endl;
            return 0;
        }
        mScreenMgr->PushScreen(UI::CreateMenuMain(mScreenMgr.get(), mRenderer));

        const int64_t msPerSec = 1000;
        const int64_t frameInterval = msPerSec / mFrameRate;
        const int64_t pollInterval = msPerSec / mPollRate;
    
        int64_t lastFrame = 0;
        int64_t lastPoll = 0;
        int64_t lastSecond = 0;
        int64_t fpsCounterLastSecond = 0;
    
        mServer->StartAccept();
        
        while(!Closed()) {
            PollInput();            
            const int64_t pollStart = SDL_GetTicks();
            if(lastPoll + pollInterval < pollStart) {
                mIO.poll();
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
                const int64_t nextTick = std::min(lastPoll + pollInterval,
                                                  lastFrame + frameInterval);
                if(delayStart < nextTick) {
                    SDL_Delay(nextTick - delayStart);
                }
            }
        }

        std::clog << "Gracefully shutdown" << std::endl;
        return 0;
    }

    bool Engine::LoadGraphics()
    {
        std::unique_ptr<UI::LoadingScreen> &&loadingScreen = UI::CreateLoadingScreen(this);

        // Entities::LoadGraphics(*mGraphicsMgr);
        // Render::LoadFonts(*mGraphicsMgr);
        // UI::LoadGraphics(*mGraphicsMgr);
        
        // mGraphicsMgr->LoadAll(loadingScreen.get());

        return loadingScreen->Exec();
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
