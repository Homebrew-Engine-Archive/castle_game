#ifndef MENU_COMBAT_H_
#define MENU_COMBAT_H_

#include <SDL.h>
#include <memory>

#include <game/screen.h>
#include <game/surface.h>

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
    
    class MenuCombat : public Screen
    {
        UI::ScreenManager *mScreenMgr;
        Render::Renderer *mRenderer;
        Surface mBackground;

        bool HandleKey(const SDL_KeyboardEvent &event);
        
    public:
        MenuCombat(UI::ScreenManager *mgr, Render::Renderer *render);
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);

        bool IsDirty(int64_t elapsed);
    };
}

#endif
