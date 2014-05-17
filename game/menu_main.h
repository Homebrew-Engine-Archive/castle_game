#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include <vector>
#include <memory>
#include <SDL.h>
#include <game/renderer.h>
#include <game/screen.h>
#include <game/button.h>

namespace Castle
{
    class Engine;
}

namespace Render
{
    class Renderer;
}

namespace UI
{
    class ScreenManager;
}

namespace UI
{
    class MenuMain : public Screen
    {
        UI::ScreenManager &mScreenManager;
        
        Surface mBackground;
    
    public:
        MenuMain(UI::ScreenManager &screenManager);
    
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };
}

#endif
