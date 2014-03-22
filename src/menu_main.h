#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#include <vector>
#include <SDL.h>
#include "renderer.h"
#include "engine.h"
#include "screen.h"
#include "button.h"

namespace Castle
{
    class Engine;
}

class MenuMain : public Screen
{
    Castle::Engine *mRoot;
    Renderer *mRenderer;
    Surface mBackground;
    std::vector<Button> mButtons;

    Button MakeCombatButton();
    
public:
    MenuMain(Castle::Engine *root);
    
    void GoCombat();
    void GoEconomics();
    void GoBuilder();
    void GoLoadGame();
    void GoExit();
    
    void Draw(Surface &frame);
    bool HandleEvent(const SDL_Event &event);
};

#endif
