#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include <game/debugconsole.h>
#include <vector>
#include <game/screen.h>

namespace UI
{
    
    class ScreenManager
    {
        // From bottom to top
        std::vector<ScreenPtr> mScreenStack;
        
    public:
        ScreenManager();

        void PushScreen(ScreenPtr &&screen);

        Screen *GetCurrentScreen();

        ScreenPtr PopScreen();

        ScreenPtr CloseScreen(Screen*);
        
        void SetCurrentScreen(ScreenPtr&&);
        
        bool HandleEvent(SDL_Event const&);

        void DrawScreen(Surface&);
        
    };
    
} // namespace UI

#endif
