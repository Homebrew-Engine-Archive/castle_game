#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include <SDL.h>

#include <game/screen.h>
#include <game/surface.h>

namespace UI
{
    class ScreenManager;
}

namespace UI
{
    class MenuMain : public Screen
    {
        UI::ScreenManager &mScreenManager;
    
    public:
        explicit MenuMain(UI::ScreenManager &screenManager);

        virtual void Render(Render::Renderer &renderer);
        virtual bool HandleEvent(const SDL_Event &event);
    };
}

#endif
