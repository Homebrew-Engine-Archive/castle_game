#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include <vector>

#include <game/screen.h>
#include <game/menu_main.h>
#include <game/menu_combat.h>
#include <game/menu_economics.h>
#include <game/menu_builder.h>
#include <game/gamescreen.h>
#include <game/console.h>
#include <game/loadingscreen.h>
#include <game/ingamemenu.h>

namespace Castle
{
    class Environment;
}

namespace UI
{
    class ScreenManager
    {
        Castle::SimulationManager &mSimulationManager;

        UI::LoadingScreen mLoadingScreen;
        UI::Console mConsole;
        UI::MenuMain mMenuMain;
        UI::GameScreen mGameScreen;
        UI::MenuCombat mMenuCombat;
        UI::InGameMenu mInGameMenu;
        
        // From bottom to top
        std::vector<Screen*> mScreenStack;
        
    public:
        ScreenManager(Castle::SimulationManager &simulationManager);

        void EnterGameScreen();
        void EnterMenuMain();
        void EnterMenuCombat();

        UI::MenuMain& MenuMain();
        UI::GameScreen& GameScreen();
        UI::Console& Console();
        UI::LoadingScreen& LoadingScreen();
        
        void PushScreen(Screen &screen);
        Screen& TopScreen();
        Screen& PopScreen();
        void CloseScreen(Screen*);
        bool HandleEvent(SDL_Event const&);
        void DrawScreen(Surface&);
    };
} // namespace UI

#endif
