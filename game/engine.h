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
#include <game/graphicsmanager.h>
#include <game/renderer.h>

namespace Castle
{
    class Engine
    {
        SDLInitializer mSDL_Init;
        Render::Renderer mRenderer;
        double mFpsAverage;
        int mFrameCounter;
        bool mClosed;
        std::chrono::milliseconds mFrameUpdateInterval;
        std::chrono::milliseconds mFpsUpdateInterval;
        bool mFpsLimited;
        boost::asio::io_service mIO;
        int16_t mPort;
        Castle::SimulationManager mSimulationMgr;
        UI::ScreenManager mScreenMgr;
        Network::Server mServer;
        Graphics::GraphicsManager mGraphicsMgr;

        bool HandleWindowEvent(SDL_WindowEvent const&);
        bool HandleKeyboardEvent(SDL_KeyboardEvent const&);
        bool HandleEvent(SDL_Event const&);
        void LoadFonts();
        void LoadGraphics();
        void PollInput();
        void PollNetwork();
        void DrawFrame();
        void UpdateFrameCounter(std::chrono::milliseconds elapsed);

    public:
        Engine();
        
        Engine(Engine const&) = delete;
        Engine& operator=(Engine const&) = delete;
    
        int Exec();
    };
}

#endif  // ENGINE_H_
