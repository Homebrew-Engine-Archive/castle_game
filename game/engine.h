#ifndef ENGINE_H_
#define ENGINE_H_

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <chrono>

#include <SDL.h>

#include <boost/asio/io_service.hpp>

#include <game/textarea.h>
#include <game/network.h>
#include <game/screenmanager.h>
#include <game/renderer.h>
#include <game/fontmanager.h>

namespace Render
{
    class RenderEngine;
}

namespace Castle
{
    class Engine
    {
        SDLInitializer mSDL_Init;
        std::unique_ptr<Render::RenderEngine> mRenderEngine;
        Render::FontManager mFontManager;
        Render::Renderer mRenderer;
        double mFpsAverage;
        int mFrameCounter;
        bool mClosed;
        std::chrono::milliseconds mFrameUpdateInterval;
        std::chrono::milliseconds mFpsUpdateInterval;
        bool mFpsLimited;
        boost::asio::io_service mIO;
        int16_t mPort;
        Network::Server mServer;
        UI::ScreenManager mScreenManager;
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

    public:
        explicit Engine();

        Engine(Engine const&);/* = delete */
        Engine& operator=(Engine const&);/* = delete */

        int Exec();
    };
}

#endif  // ENGINE_H_
