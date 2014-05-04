#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include <game/debugconsole.h>
#include <vector>
#include <game/screen.h>

#include <game/menu_main.h>
#include <game/menu_combat.h>
#include <game/menu_economics.h>
#include <game/menu_builder.h>
#include <game/gamescreen.h>
#include <game/debugconsole.h>

namespace Render
{
    class Renderer;
}

namespace UI
{
    class ScreenManager
    {
        Render::Renderer *mRenderer;

        UI::DebugConsole mConsole;
        UI::MenuMain mMenuMain;
        UI::GameScreen mGameScreen;
        UI::MenuCombat mMenuCombat;
        
        // From bottom to top
        std::vector<Screen*> mScreenStack;
        
    public:
        ScreenManager(Render::Renderer *renderer);

        void EnterGameScreen();
        void EnterMenuMain();
        void EnterMenuCombat();

        UI::MenuMain& MenuMain();
        UI::GameScreen& GameScreen();
        UI::DebugConsole& DebugConsole();
        
        void PushScreen(Screen *screen);
        Screen* TopScreen();
        Screen* PopScreen();
        void CloseScreen(Screen*);
        bool HandleEvent(SDL_Event const&);
        void DrawScreen(Surface&);
    };
} // namespace UI

#endif
