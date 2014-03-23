#include "engine.h"

#include <sstream>
#include <string>
#include <boost/asio/io_service.hpp>
#include <vector>
#include <SDL.h>

#include "screenmanager.h"
#include "renderer.h"
#include "menu_combat.h"
#include "menu_main.h"
#include "loadingscreen.h"
#include "macrosota.h"
#include "network.h"
#include "debugconsole.h"

namespace Castle
{

    class EngineImpl final : public Engine
    {
        Render::Renderer *mRenderer;
        double mFpsAverage;
        std::uint64_t mFrameCounter;
        bool mClosed;
        int mFrameRate;
        bool mFpsLimited;
        bool mShowConsole;
        std::unique_ptr<GUI::ScreenManager> mScreenMgr;
        
        bool HandleWindowEvent(const SDL_WindowEvent &event);
        bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
        GUI::Screen *GetCurrentScreen() const;
        void ToggleConsole();
        void DrawFrame();
        bool HandleEvent(const SDL_Event &event);
    
    public:
        EngineImpl(Render::Renderer *mRenderer);
        EngineImpl(const EngineImpl &) = delete;
        EngineImpl(EngineImpl &&) = default;
        EngineImpl &operator=(const EngineImpl &) = delete;
        EngineImpl &operator=(EngineImpl &&) = default;
        ~EngineImpl() = default;
    
        int Exec();
        void SetCurrentScreen(GUI::ScreenPtr &&screen);
        void PushScreen(GUI::ScreenPtr &&screen);
        GUI::ScreenPtr PopScreen();
        Render::Renderer *GetRenderer();

        bool Closed() const;
        void PollInput();
    };
    
    EngineImpl::EngineImpl(Render::Renderer *renderer)
        : mRenderer(renderer)
        , mFpsAverage(0.0f)
        , mFrameCounter(0.0f)
        , mClosed(false)
        , mFrameRate(16)
        , mFpsLimited(true)
        , mShowConsole(false)
        , mScreenMgr(make_unique<GUI::ScreenManager>(this))
    { }

    bool EngineImpl::HandleWindowEvent(const SDL_WindowEvent &window)
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

    bool EngineImpl::HandleKeyboardEvent(const SDL_KeyboardEvent &key)
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

    void EngineImpl::ToggleConsole()
    {
        mShowConsole = !mShowConsole;
        mScreenMgr->ShowConsole(mShowConsole);
    }

    bool EngineImpl::Closed() const
    {
        return mClosed;
    }
    
    GUI::Screen *EngineImpl::GetCurrentScreen() const
    {
        return mScreenMgr->GetCurrentScreen();
    }

    void EngineImpl::SetCurrentScreen(GUI::ScreenPtr &&screen)
    {
        mScreenMgr->SetCurrentScreen(std::move(screen));
    }

    void EngineImpl::PushScreen(GUI::ScreenPtr &&screen)
    {
        mScreenMgr->PushScreen(std::move(screen));
    }

    GUI::ScreenPtr EngineImpl::PopScreen()
    {
        return mScreenMgr->PopScreen();
    }

    void EngineImpl::PollInput()
    {
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
    
    int EngineImpl::Exec()
    {
        if(!RunLoadingScreen(this)) {
            std::clog << "Loading has been interrupted."
                      << std::endl;
            return false;
        }

        PushScreen(GUI::CreateMenuMain(this));

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

    void EngineImpl::DrawFrame()
    {
        Surface frame = mRenderer->BeginFrame();

        mScreenMgr->DrawScreen(frame);
    
        SDL_Color color = MakeColor(128, 128, 255, 128);
        mRenderer->SetColor(color);
        mRenderer->SetFont("font_stronghold_aa", 24);

        SDL_Point pos = ShiftPoint(TopLeft(mRenderer->GetOutputSize()), 5, 5);
    
        std::ostringstream oss;
        oss << "FPS: " << mFpsAverage;
        mRenderer->RenderTextLine(oss.str(), pos);

        mRenderer->EndFrame();
    }

    bool EngineImpl::HandleEvent(const SDL_Event &event)
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

    Render::Renderer *EngineImpl::GetRenderer()
    {
        return mRenderer;
    }

    std::unique_ptr<Engine> CreateEngine(Render::Renderer *renderer)
    {
        return make_unique<EngineImpl>(renderer);
    }

}
