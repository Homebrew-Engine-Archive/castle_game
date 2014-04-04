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
    
    class MenuMain : public Screen
    {
        UI::ScreenManager *mScreenMgr;
        Render::Renderer *mRenderer;
        Surface mBackground;
    
    public:
        MenuMain(UI::ScreenManager *mgr, Render::Renderer *render);
    
        void GoCombat();
        void GoEconomics();
        void GoBuilder();
        void GoLoadGame();
        void GoExit();
    
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);

        bool IsDirty(int64_t elapsed);
    };

    std::unique_ptr<MenuMain> CreateMenuMain(UI::ScreenManager *mgr, Render::Renderer *render);
}

#endif
