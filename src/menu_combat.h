#ifndef MENU_COMBAT_H_
#define MENU_COMBAT_H_

#include "SDL.h"

#include "rootscreen.h"
#include "screen.h"
#include "surface.h"

class MenuCombat : public Screen
{
    RootScreen *m_root;
    Renderer *m_renderer;
    Surface m_background;
    
public:
    MenuCombat(RootScreen *root);
    void Draw(Surface frame);
    bool HandleEvent(const SDL_Event &event);
};

#endif
