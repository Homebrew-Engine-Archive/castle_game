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

        ScreenPtr CloseScreen(Screen *screen);
        
        void SetCurrentScreen(ScreenPtr &&screen);
        
        bool HandleEvent(const SDL_Event &event);

        void DrawScreen(Surface &frame);
        
    };
    
} // namespace UI

#endif
