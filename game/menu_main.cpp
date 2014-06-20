#include "menu_main.h"

#include <game/menu_combat.h>
#include <game/screenmanager.h>

namespace castle
{
    namespace ui
    {
        MenuMain::MenuMain(ScreenManager &screenManager)
            : mScreenManager(screenManager)
        {
        }

        bool MenuMain::HandleEvent(const SDL_Event &event)
        {
            return false;
        }

        void MenuMain::Render(render::Renderer &renderer)
        {
        }
    } // namespace ui
}
