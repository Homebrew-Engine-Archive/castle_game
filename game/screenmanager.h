#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include <string>
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

namespace castle
{
    namespace ui
    {
        class TextArea;
    }
    
    namespace render
    {
        class renderer;
    }
    
    namespace ui
    {
        class ScreenManager
        {
            Console mConsole;
            MenuMain mMenuMain;
            GameScreen mGameScreen;
        
            // From bottom to top
            std::vector<Screen*> mScreenStack;
            std::unique_ptr<ui::TextArea> mInfoArea;
        
        public:
            ScreenManager(ScreenManager&&) = delete;
            ScreenManager& operator=(ScreenManager&&) = delete;
            ScreenManager(ScreenManager const&) = delete;
            ScreenManager& operator=(ScreenManager const&) = delete;

            explicit ScreenManager();
            virtual ~ScreenManager() = default;

            void ToggleConsole();
        
            void EnterGameScreen();
            void EnterMenuMain();

            MenuMain& GetMenuMain();
            GameScreen& GetGameScreen();
            Console& GetConsole();

            void SetTestString(std::string);
            void PushScreen(Screen &screen);
            Screen& TopScreen();
            Screen& PopScreen();
            void CloseScreen(Screen*);
            bool HandleEvent(SDL_Event const&);
            void Render(render::Renderer &renderer);
        };
    } // namespace ui
}

#endif
