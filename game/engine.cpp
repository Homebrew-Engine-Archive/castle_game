#include "engine.h"

#include <iomanip>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <thread>

#include <SDL.h>

#include <game/collection.h>
#include <game/color.h>
#include <game/gamemap.h>
#include <game/gamescreen.h>
#include <game/network.h>
#include <game/point.h>
#include <game/rect.h>
#include <game/renderengine.h>
#include <game/renderer.h>
#include <game/screen.h>
#include <game/screenmanager.h>
#include <game/sdl_utils.h>
#include <game/simulationmanager.h>

namespace
{
    constexpr std::chrono::milliseconds Elapsed(const std::chrono::steady_clock::time_point &lhs, const std::chrono::steady_clock::time_point &rhs)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(rhs - lhs);
    }
}

namespace castle
{
    Engine::~Engine() = default;
    Engine::Engine()
        : mSDL_Init(new SDLInitializer(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE))
        , mRenderer(new render::Renderer)
        , mSimManager(new world::SimulationManager)
        , mFrameUpdateInterval(std::chrono::milliseconds(0))
        , mFpsUpdateInterval(std::chrono::seconds(3))
        , mLastSimPoll()
        , mLastRenderPoll()
        , mFpsLimited(false)
        , mPort(4500)
        , mFpsAverage(0.0f)
        , mFrameCounter(0)
        , mClosed(false)
        , mServer(new net::Server(mPort))
        , mScreenManager(new ui::ScreenManager)
    {
        mScreenManager->SetTestString("No FPS for you, Sir");
    }

    bool Engine::HandleWindowEvent(const SDL_WindowEvent &window)
    {
        switch(window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            {
                const int width = window.data1;
                const int height = window.data2;
                ResizeScreen(width, height);
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
        case SDLK_TAB:
            mScreenManager->ToggleConsole();
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

    void Engine::ResizeScreen(int width, int height)
    {
        mRenderer->SetScreenWidth(width);
        mRenderer->SetScreenHeight(height);
    }
    
    void Engine::LoadFonts()
    {
        const int minHeight = 8;
        const int maxHeight = 42;
        
        const std::string family = "DejaVuSans";

        for(int h = minHeight; h <= maxHeight; ++h) {
            try {
                mRenderer->LoadFont(core::Font(family, h));
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
                if(!mScreenManager->TopScreen().HandleEvent(event)) {
                    HandleEvent(event);
                }
            }
        } catch(const std::exception &error) {
            std::cerr << "exception on input handling: " << error.what() << std::endl;
        }
    }
    
    void Engine::PollNetIO()
    {
        try {
            mServer->Poll();
        } catch(const castle::net::receive_error &error) {
            /** There we should probably reject some connection
                and report simulation manager a network issue
            **/
            std::cerr << error.what() << std::endl;
        } catch(const castle::net::accept_error &error) {
            /** That is an interesting one.
                One solution is report an issue to the user and reset screen to main menu.
                The other is quietly reset underlying io_service and pray for win. **/
            std::cerr << error.what() << std::endl;
        } catch(const std::exception &error) {
            std::cerr << "exception on polling network: " << error.what() << std::endl;
        }
        
        // forward connections to simulation manager
        // forward data to simulation manager
    }

    void Engine::UpdateFrameCounter(std::chrono::milliseconds elapsed)
    {
        const std::chrono::milliseconds oneSecond = std::chrono::seconds(1);
        const double preciseFrameCounter = mFrameCounter * oneSecond.count();
        mFpsAverage = preciseFrameCounter / elapsed.count();
        mFrameCounter = 0;

        std::ostringstream oss;
        oss << "Your FPS, sir: " << std::setw(10) << mFpsAverage;
        mScreenManager->SetTestString(oss.str());
    }

    void Engine::PollRenderQueue()
    {
        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        const std::chrono::milliseconds sinceLastRender = Elapsed(now, mLastRenderPoll);
        if(!mFpsLimited || sinceLastRender >= mFrameUpdateInterval) {
            mLastRenderPoll = now;
            mRenderer->BeginFrame();
            mScreenManager->Render(*mRenderer);
            mRenderer->EndFrame();
            mFrameCounter += 1;
        }
    }

    void Engine::PollSimulationQueue()
    {
        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        const std::chrono::milliseconds sinceLastSim = Elapsed(mLastSimPoll, now);
        if(mSimManager->HasUpdate(sinceLastSim)) {
            mLastSimPoll = now;
            mSimManager->Update(sinceLastSim);
        }
    }    

    void Engine::LoadSimulationContext()
    {
        std::unique_ptr<world::Map> testMap(new world::Map(100));
        GenerateTestMap(*testMap);
        
        world::SimulationContext &context = mSimManager->PrimaryContext();
        context.SetMap(std::move(testMap));
    }
    
    int Engine::Exec()
    {
        LoadFonts();
        LoadGraphics();
        LoadSimulationContext();

        mScreenManager->GetGameScreen().SetSimulationContext(mSimManager->PrimaryContext());

        mScreenManager->EnterGameScreen();
        mServer->StartAccept();
        
        std::chrono::steady_clock::time_point prevFpsUpdate = std::chrono::steady_clock::now();
        
        while(!mClosed) {
            PollInput();
            PollNetIO();
            PollRenderQueue();
            PollSimulationQueue();
            
            const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            const std::chrono::milliseconds sinceLastFpsUpdate = Elapsed(prevFpsUpdate, now);
            if(mFpsUpdateInterval < sinceLastFpsUpdate) {
                prevFpsUpdate = now;
                UpdateFrameCounter(sinceLastFpsUpdate);
            }
        }
        return 0;
    }
}
