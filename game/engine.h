#ifndef ENGINE_H_
#define ENGINE_H_

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <chrono>

#include <SDL.h>

class SDLInitializer;

namespace castle
{
    namespace world
    {
        class SimulationManager;
    }
    
    namespace net
    {
        class Server;
    }

    namespace render
    {
        class RenderEngine;
        class Renderer;
        class FontEngine;
    }
    
    namespace ui
    {
        class ScreenManager;
    }
}

namespace castle
{
    class Engine
    {
        std::unique_ptr<SDLInitializer> mSDL_Init;
        std::unique_ptr<render::Renderer> mRenderer;
        std::unique_ptr<world::SimulationManager> mSimManager;
        std::chrono::milliseconds mFrameUpdateInterval;
        std::chrono::milliseconds mFpsUpdateInterval;
        std::chrono::steady_clock::time_point mLastSimPoll;
        std::chrono::steady_clock::time_point mLastRenderPoll;
        bool mFpsLimited;
        int16_t mPort;
        double mFpsAverage;
        int mFrameCounter;
        bool mClosed;
        std::unique_ptr<net::Server> mServer;
        std::unique_ptr<ui::ScreenManager> mScreenManager;

    private:
        void ResizeScreen(int width, int height);
        bool HandleWindowEvent(const SDL_WindowEvent &event);
        bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
        bool HandleEvent(const SDL_Event &event);
        void LoadFonts();
        void LoadGraphics();
        void PollInput();
        void PollNetIO();
        void PollRenderQueue();
        void PollSimulationQueue();
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
