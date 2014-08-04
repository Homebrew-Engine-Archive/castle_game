#include "menu_combat.h"

#include <SDL.h>

#include <game/collection.h>
#include <core/sdl_utils.h>
#include <game/screenmanager.h>

namespace castle
{
    namespace ui
    {
        MenuCombat::MenuCombat(ScreenManager &screenManager)
            : mScreenManager(screenManager)
        {
        }

        bool MenuCombat::HandleKey(const SDL_KeyboardEvent &event)
        {
            switch(event.keysym.sym) {
            case SDLK_ESCAPE:
                mScreenManager.CloseScreen(this);
                return true;
            }
            return false;
        }
    
        bool MenuCombat::HandleEvent(const SDL_Event &event)
        {
            switch(event.type) {
            case SDL_KEYDOWN:
                return HandleKey(event.key);
            }
            return false;
        }
    } // namespace ui
}
