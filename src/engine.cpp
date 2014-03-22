#include "engine.h"

#include <sstream>
#include <string>
#include <boost/asio/io_service.hpp>
#include <vector>
#include <SDL.h>

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
        Renderer *mRenderer;
        double mFpsAverage;
        std::uint64_t mFrameCounter;
        bool mClosed;
        int mFrameRate;
        bool mFpsLimited;
        bool mShowConsole;
        ScreenPtr mDebugConsole;
        std::vector<ScreenPtr> mScreenStack;
        
        bool HandleWindowEvent(const SDL_WindowEvent &event);
        bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
        Screen *GetCurrentScreen() const;
        void ToggleConsole();
        void DrawFrame();
        bool HandleEvent(const SDL_Event &event);
    
      public:
        EngineImpl(Renderer *mRenderer);
        EngineImpl(const EngineImpl &) = delete;
        EngineImpl(EngineImpl &&) = default;
        EngineImpl &operator=(const EngineImpl &) = delete;
        EngineImpl &operator=(EngineImpl &&) = default;
    
        int Exec();
        void SetCurrentScreen(ScreenPtr &&screen);
        void PushScreen(ScreenPtr &&screen);
        ScreenPtr PopScreen();
        Renderer *GetRenderer();

        inline bool Closed() const { return mClosed; }
        void PollInput();
        
    };

    EngineImpl::EngineImpl(Renderer *renderer)
        : mRenderer(renderer)
        , mFpsAverage(0.0f)
        , mFrameCounter(0.0f)
        , mClosed(false)
        , mFrameRate(16)
        , mFpsLimited(true)
        , mShowConsole(false)
        , mDebugConsole(std::move(GUI::CreateDebugConsole(this)))
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

        if(mShowConsole) {
            PushScreen(move(mDebugConsole));
        } else {
            mDebugConsole = move(PopScreen());
        }
    }

    Screen *EngineImpl::GetCurrentScreen() const
    {
        if(mScreenStack.empty())
            return NULL;
        else
            return mScreenStack.back().get();
    }

    void EngineImpl::SetCurrentScreen(ScreenPtr &&screen)
    {
        mScreenStack.clear();
        PushScreen(move(screen));
    }

    void EngineImpl::PushScreen(ScreenPtr &&screen)
    {
        mScreenStack.push_back(move(screen));
    }

    ScreenPtr EngineImpl::PopScreen()
    {
        if(!mScreenStack.empty()) {
            ScreenPtr ptr = std::move(mScreenStack.back());
            mScreenStack.pop_back();
            return ptr;
        }

        return ScreenPtr(nullptr);
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

        PushScreen(make_unique<MenuMain>(this));

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
            const int64_t frameInterval = msPerSec / mFrameRate;
            const int64_t pollStart = SDL_GetTicks();
            if(lastPoll + pollInterval < pollStart) {
                io.poll();
                lastPoll = pollStart;
            }

            if(GetCurrentScreen() == NULL) {
                throw std::runtime_error("No current screen");
            }
        
            PollInput();

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

        for(ScreenPtr &ptr : mScreenStack) {
            if(ptr) {
                ptr->Draw(frame);
            } else {
                // NOTE
                // It's assumed to be impossible
                throw std::runtime_error("empty screen in screen stack");
            }
        }
    
        SDL_Color color = MakeColor(255, 255, 255, 255);
        mRenderer->SetColor(color);
        mRenderer->SetFont(
            "font_stronghold_aa",
            24);

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

    Renderer *EngineImpl::GetRenderer()
    {
        return mRenderer;
    }

    std::unique_ptr<Engine> CreateEngine(Renderer *renderer)
    {
        return make_unique<EngineImpl>(renderer);
    }

}
