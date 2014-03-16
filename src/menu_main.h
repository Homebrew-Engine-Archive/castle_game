#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include <vector>
#include "SDL.h"
#include "renderer.h"
#include "rootscreen.h"
#include "screen.h"
#include "button.h"

class MenuMain : public Screen
{
    RootScreen *m_root;
    Renderer *m_renderer;
    Surface m_background;
    std::vector<Button> m_buttons;

    Button MakeCombatButton();
    
public:
    MenuMain(RootScreen *root);
    ~MenuMain();
    
    void GoCombat();
    void GoEconomics();
    void GoBuilder();
    void GoLoadGame();
    void GoExit();
    
    void Draw(Surface frame);
    bool HandleEvent(const SDL_Event &event);
};

#endif
