#ifndef MENU_COMBAT_H_
#define MENU_COMBAT_H_

#include <SDL.h>
#include <memory>

#include <game/screen.h>
#include <game/surface.h>

namespace Render
{
    class FontManager;
}

namespace UI
{
    class ScreenManager;
    
    class MenuCombat : public Screen
    {
        UI::ScreenManager &mScreenManager;
        Surface mBackground;

        bool HandleKey(const SDL_KeyboardEvent &event);
        
    public:
        MenuCombat(UI::ScreenManager &screenManager);
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };
}

#endif
