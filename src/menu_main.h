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
    RootScreen *mRoot;
    Renderer *mRenderer;
    Surface mBackground;
    std::vector<Button> mButtons;

    Button MakeCombatButton();
    
public:
    MenuMain(RootScreen *root);
    
    void GoCombat();
    void GoEconomics();
    void GoBuilder();
    void GoLoadGame();
    void GoExit();
    
    void Draw(Surface &frame);
    bool HandleEvent(const SDL_Event &event);
};

#endif
