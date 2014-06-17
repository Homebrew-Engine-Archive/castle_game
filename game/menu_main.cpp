#include "menu_main.h"

#include <game/menu_combat.h>
#include <game/screenmanager.h>

namespace Castle
{
    namespace UI
    {
        MenuMain::MenuMain(ScreenManager &screenManager)
            : mScreenManager(screenManager)
        {
        }

        bool MenuMain::HandleEvent(const SDL_Event &event)
        {
            return false;
        }

        void MenuMain::Render(Render::Renderer &renderer)
        {
        }
    } // namespace UI
}
