#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include "debugconsole.h"
#include <vector>
#include "screen.h"

namespace Castle
{
    class Engine;
}

namespace UI
{
    
    class ScreenManager
    {
        Castle::Engine *mEngine;

        // From bottom to top
        std::vector<ScreenPtr> mScreenStack;
        
    public:
        ScreenManager(Castle::Engine *engine);

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
