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

#include <game/network.h>
#include <game/fontmanager.h>
#include <game/screenmanager.h>
#include <game/simulationmanager.h>

namespace Castle
{
    class Engine
    {
        class Render::Renderer &mRenderer;
        double mFpsAverage;
        double mFpsAverageMax;
        int64_t mFrameCounter;
        bool mClosed;
        std::chrono::milliseconds mFrameUpdateInterval;
        std::chrono::milliseconds mFpsUpdateInterval;
        bool mFpsLimited;
        bool mShowConsole;
        boost::asio::io_service mIO;
        int16_t mPort;
        Render::FontManager mFontMgr;
        Castle::SimulationManager mSimulationMgr;
        UI::ScreenManager mScreenMgr;
        Network::Server mServer;

        bool HandleWindowEvent(SDL_WindowEvent const&);
        bool HandleKeyboardEvent(SDL_KeyboardEvent const&);
        bool HandleEvent(SDL_Event const&);
        void LoadFonts();
        void PollInput();
        void DrawFrame();
        void UpdateFrameCounter(std::chrono::milliseconds elapsed);

    public:
        explicit Engine(Render::Renderer&);
        
        Engine(Engine const&) = delete;
        Engine& operator=(Engine const&) = delete;
    
        int Exec();
    };
}

#endif  // ENGINE_H_
