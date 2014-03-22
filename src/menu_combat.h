#ifndef MENU_COMBAT_H_
#define MENU_COMBAT_H_

#include <SDL.h>

#include "engine.h"
#include "screen.h"
#include "surface.h"

namespace Castle
{
    class Engine;
}

class MenuCombat : public Screen
{
    Castle::Engine *mRoot;
    Renderer *mRenderer;
    Surface mBackground;
    
public:
    MenuCombat(Castle::Engine *root);
    void Draw(Surface &frame);
    bool HandleEvent(const SDL_Event &event);
};

#endif
