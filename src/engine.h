#ifndef ENGINE_H_
#define ENGINE_H_

#include <iostream>
#include <sstream>
#include <memory>
#include <SDL.h>
#include "screen.h"
#include "screenmanager.h"

namespace Render
{
    class Renderer;
}

namespace Castle
{

    class Engine
    {
        Render::Renderer *mRenderer;
        double mFpsAverage;
        std::uint64_t mFrameCounter;
        bool mClosed;
        int mFrameRate;
        bool mFpsLimited;
        bool mShowConsole;
        UI::Screen *mConsolePtr;
        std::stringstream mConsoleInput;
        std::stringstream mConsoleOutput;
        std::unique_ptr<UI::ScreenManager> mScreenMgr;
        
        bool HandleWindowEvent(const SDL_WindowEvent &event);
        bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
        UI::Screen *GetCurrentScreen() const;
        void DrawFrame();
        bool HandleEvent(const SDL_Event &event);
        void ToggleConsole();
    
    public:
        Engine(Render::Renderer *mRenderer);
        Engine(const Engine&) = delete;
        Engine(Engine&&) = default;
        Engine &operator=(const Engine&) = delete;
        Engine &operator=(Engine&&) = default;
        ~Engine() = default;
    
        int Exec();
        void SetCurrentScreen(UI::ScreenPtr &&screen);
        void PushScreen(UI::ScreenPtr &&screen);
        UI::ScreenPtr PopScreen();
        Render::Renderer *GetRenderer();

        bool Closed() const;
        void PollInput();
    };
    
}

#endif
