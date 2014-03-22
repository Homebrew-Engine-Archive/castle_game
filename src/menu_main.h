#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include <vector>
#include <memory>
#include <SDL.h>
#include "renderer.h"
#include "engine.h"
#include "screen.h"
#include "button.h"

namespace Castle
{
    class Engine;
}

namespace Render
{
    class Renderer;
}

namespace GUI
{

    class MenuMain : public Screen
    {
        Castle::Engine *mEngine;
        Render::Renderer *mRenderer;
        Surface mBackground;
        std::vector<Button> mButtons;

        Button MakeCombatButton();
    
    public:
        MenuMain(Castle::Engine *engine);
    
        void GoCombat();
        void GoEconomics();
        void GoBuilder();
        void GoLoadGame();
        void GoExit();
    
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };

    std::unique_ptr<MenuMain> CreateMenuMain(Castle::Engine *engine);
}

#endif
