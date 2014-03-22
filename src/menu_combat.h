#ifndef MENU_COMBAT_H_
#define MENU_COMBAT_H_

#include <SDL.h>
#include <memory>

#include "screen.h"
#include "surface.h"

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

    class MenuCombat : public Screen
    {
        Castle::Engine *mEngine;
        Render::Renderer *mRenderer;
        Surface mBackground;
    
    public:
        MenuCombat(Castle::Engine *engine);
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };


    std::unique_ptr<MenuCombat> CreateMenuCombat(Castle::Engine *engine);
}

#endif
