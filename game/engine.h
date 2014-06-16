#ifndef ENGINE_H_
#define ENGINE_H_

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <chrono>

#include <SDL.h>

class SDLInitializer;

namespace Network
{
    class Server;
}

namespace UI
{
    class ScreenManager;
    class TextArea;
}

namespace Render
{
    class RenderEngine;
    class Renderer;
    class FontManager;
}

namespace Castle
{
    class SimulationManager;
}

namespace Castle
{
    class Engine
    {
        std::unique_ptr<SDLInitializer> mSDL_Init;
        std::unique_ptr<Render::RenderEngine> mRenderEngine;
        std::unique_ptr<Render::FontManager> mFontManager;
        std::unique_ptr<Render::Renderer> mRenderer;
        std::unique_ptr<Castle::SimulationManager> mSimManager;
        double mFpsAverage;
        int mFrameCounter;
        bool mClosed;
        std::chrono::milliseconds mFrameUpdateInterval;
        std::chrono::milliseconds mFpsUpdateInterval;
        bool mFpsLimited;
        int16_t mPort;
        std::unique_ptr<Network::Server> mServer;
        std::unique_ptr<UI::ScreenManager> mScreenManager;
        std::unique_ptr<UI::TextArea> mInfoArea;

    private:
        void ResizeScreen(int width, int height);
        bool HandleWindowEvent(const SDL_WindowEvent &event);
        bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
        bool HandleEvent(const SDL_Event &event);
        void LoadFonts();
        void LoadGraphics();
        void PollInput();
        void PollNetwork();
        void DrawFrame();
        void LoadSimulationContext();
        void UpdateFrameCounter(std::chrono::milliseconds elapsed);

    public:
        explicit Engine();

        Engine(Engine const&) = delete;
        Engine& operator=(Engine const&) = delete;
        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        virtual ~Engine();

        int Exec();
    };
}

#endif  // ENGINE_H_
