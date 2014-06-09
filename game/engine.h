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

#include <game/textarea.h>
#include <game/network.h>
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
        UI::ScreenManager mScreenManager;
        Network::Server mServer;
        Graphics::GraphicsManager mGraphicsMgr;
        UI::TextArea mInfoArea;

        void ResizeScreen(int width, int height);
        bool HandleWindowEvent(SDL_WindowEvent const&);
        bool HandleKeyboardEvent(SDL_KeyboardEvent const&);
        bool HandleEvent(SDL_Event const&);
        void LoadFonts();
        void LoadGraphics();
        void PollInput();
        void PollNetwork();
        void DrawFrame();
        void UpdateFrameCounter(std::chrono::milliseconds elapsed);

    protected:
        void FrameStarted(std::chrono::milliseconds delta);
        void IncommingConnection(Network::Connection &connection);
        void ConnectionLost(Network::Connection &connection);
        void ConnectionData(Network::Connection &connection);
        
    public:
        Engine();
        
        Engine(Engine const&) = delete;
        Engine& operator=(Engine const&) = delete;
    
        int Exec();
    };
}

#endif  // ENGINE_H_
