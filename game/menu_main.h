#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include <SDL.h>

#include <game/screen.h>

namespace castle
{
    namespace ui
    {
        class ScreenManager;
    }

    namespace ui
    {
        class MenuMain : public Screen
        {
            ScreenManager &mScreenManager;
    
        public:
            explicit MenuMain(ScreenManager &screenManager);
            virtual ~MenuMain() = default;

            virtual void Render(render::Renderer &renderer);
            virtual bool HandleEvent(const SDL_Event &event);
        };
    }
}

#endif
