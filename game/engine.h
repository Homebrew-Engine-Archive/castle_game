#ifndef ENGINE_H_
#define ENGINE_H_

#include <iostream>
#include <sstream>
#include <memory>
#include <SDL.h>

#include <boost/asio.hpp>

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
    class Engine
    {
        Render::Renderer *mRenderer;
        double mFpsAverage;
        uint64_t mFrameCounter;
        bool mClosed;
        int64_t mFrameRate;
        bool mFpsLimited;
        bool mShowConsole;
        int64_t mPollRate;
        UI::Screen *mConsolePtr;
        std::unique_ptr<Render::FontManager> mFontMgr;
        std::unique_ptr<UI::ScreenManager> mScreenMgr;
        boost::asio::io_service mIO;
        int16_t mPort;
        std::unique_ptr<Network::Server> mServer;
        
        bool HandleWindowEvent(SDL_WindowEvent const&);
        bool HandleKeyboardEvent(SDL_KeyboardEvent const&);
        void DrawFrame();
        bool HandleEvent(SDL_Event const&);
        void ToggleConsole();
        void LoadFonts();
    
    public:
        explicit Engine(Render::Renderer*);
        Engine(Engine const&) = delete;
        Engine &operator=(Engine const&) = delete;
        virtual ~Engine();
    
        int Exec();

        bool Closed() const;
        void PollInput();
    };
}

#endif  // ENGINE_H_
