#include "engine.h"

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>

#include <SDL.h>

#include <game/make_unique.h>
#include <game/fontmanager.h>
#include <game/textrenderer.h>
#include <game/gamescreen.h>
#include <game/renderer.h>
#include <game/menu_combat.h>
#include <game/menu_main.h>
#include <game/loadingscreen.h>
#include <game/debugconsole.h>
#include <game/screen.h>
#include <game/entityclass.h>
#include <game/network.h>
#include <game/screenmanager.h>

namespace Castle
{

    Engine::~Engine() = default;
    
    Engine::Engine(Render::Renderer *renderer)
        : mRenderer(renderer)
        , mFpsAverage(0.0f)
        , mFrameCounter(0)
        , mClosed(false)
        , mFrameRate(50)
        , mFpsLimited(false)
        , mShowConsole(false)
        , mPollRate(66)
        , mConsolePtr(nullptr)
        , mFontMgr(new Render::FontManager)
        , mScreenMgr(new UI::ScreenManager)
        , mIO()
        , mPort(4500)
        , mServer(new Network::Server(mIO, mPort))
    { }

    bool Engine::HandleWindowEvent(const SDL_WindowEvent &window)
    {
        switch(window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            {
                int width = window.data1;
                int height = window.data2;
                mRenderer->SetWindowSize(width, height);
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
        LoadFonts();
                
        // mScreenMgr->PushScreen(
        //     ScreenPtr(
        //         new UI::MenuMain(mScreenMgr.get(), mRenderer)));
        
        mScreenMgr->PushScreen(
            UI::ScreenPtr(
                new UI::GameScreen(mScreenMgr.get(), mRenderer)));

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

        return 0;
    }

    void Engine::LoadFonts()
    {
        static int sizes[] = {8, 9, 11, 13, 15, 17, 19, 23, 25, 30, 45};
        static std::string families[] = {Render::FontStronghold};
        for(std::string family : families) {
            for(int fsize : sizes) {
                mFontMgr->LoadFont(family, fsize);
            }
        }
    }
        
    void Engine::DrawFrame()
    {
        Surface frame = mRenderer->BeginFrame();
        mScreenMgr->DrawScreen(frame);

        std::ostringstream oss;
        oss << "(Castle game project) " << "FPS: " << mFpsAverage;
        std::string text = oss.str();

        Render::TextRenderer textRenderer(frame);
        textRenderer.SetFont(mFontMgr->Font(Render::FontStronghold, 10));
        textRenderer.SetCursorMode(Render::CursorMode::BaseLine);
        textRenderer.Translate(0, 20);
        textRenderer.SetColor(MakeColor(255, 255, 255, 127));
        textRenderer.PutString(text);

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
}
