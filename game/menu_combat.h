#ifndef MENU_COMBAT_H_
#define MENU_COMBAT_H_

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
        class MenuCombat : public Screen
        {
            ScreenManager &mScreenManager;
            bool HandleKey(const SDL_KeyboardEvent &event);
        
        public:
            explicit MenuCombat(ScreenManager &screenManager);
            virtual ~MenuCombat() = default;
        
            bool HandleEvent(const SDL_Event &event);
        };
    }
}

#endif
