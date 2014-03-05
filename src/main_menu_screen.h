#ifndef MAIN_MENU_SCREEN_H_
#define MAIN_MENU_SCREEN_H_

#include <functional>
#include <SDL2/SDL.h>

#include "widget.h"
#include "renderer.h"
#include "rootscreen.h"
#include "screen.h"
#include "button.h"

class ButtonCombat : public Button
{

};

class ButtonEconomics : public Button
{

};

class ButtonMapEdit

class MainMenuScreen : public Screen
{
    RootScreen &m_root;
    Renderer &m_renderer;
    std::vector<Button> m_buttons;
    Surface m_background;
    
public:
    MainMenuScreen(RootScreen &root, Renderer &renderer);
    ~MainMenuScreen();

    void GoCombat();
    void GoEconomics();
    void GoMapEditor();
    void GoLoadGame();
    void Close();
    
    void Draw();
    bool HandleEvent(const SDL_Event &event);
};

#endif
