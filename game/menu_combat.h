#ifndef MENU_COMBAT_H_
#define MENU_COMBAT_H_

#include <SDL.h>
#include <memory>

#include <game/screen.h>
#include <game/surface.h>

namespace Render
{
    class Renderer;
    class FontManager;
}

namespace UI
{
    class ScreenManager;
    
    class MenuCombat : public Screen
    {
        Render::Renderer &mRenderer;
        Render::FontManager &mFontManager;
        UI::ScreenManager &mScreenManager;
        Surface mBackground;

        bool HandleKey(const SDL_KeyboardEvent &event);
        
    public:
        MenuCombat(Render::Renderer &renderer, Render::FontManager &fontManager, UI::ScreenManager &screenManager);
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };
}

#endif
