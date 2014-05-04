#ifndef ENGINE_H_
#define ENGINE_H_

#include <iostream>
#include <sstream>
#include <memory>
#include <chrono>
#include <SDL.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>

namespace UI
{
    class ScreenManager;
    class Screen;
}

namespace Network
{
    class Server;
}

namespace Render
{
    class FontManager;
    class Renderer;
}

namespace Castle
{
    class SimulationManager;
    
    class Engine
    {
        Render::Renderer *mRenderer;
        double mFpsAverage;
        int64_t mFrameCounter;
        bool mClosed;
        int64_t mFrameRate;
        bool mFpsLimited;
        bool mShowConsole;
        int64_t mPollRate;
        boost::asio::io_service mIO;
        int16_t mPort;
        std::unique_ptr<Render::FontManager> mFontMgr;
        std::unique_ptr<UI::ScreenManager> mScreenMgr;
        std::unique_ptr<Castle::SimulationManager> mSimulationMgr;
        std::unique_ptr<Network::Server> mServer;

        bool HandleWindowEvent(SDL_WindowEvent const&);
        bool HandleKeyboardEvent(SDL_KeyboardEvent const&);
        bool HandleEvent(SDL_Event const&);
        void LoadFonts();
        void PollInput();
        void DrawFrame();        

    public:
        explicit Engine(Render::Renderer*);
        Engine(Engine const&) = delete;
        Engine &operator=(Engine const&) = delete;
        virtual ~Engine();
    
        int Exec();
    };
}

#endif  // ENGINE_H_
