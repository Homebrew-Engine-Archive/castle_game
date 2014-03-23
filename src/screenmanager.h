#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include <vector>
#include "screen.h"

namespace Castle
{
    class Engine;
}

namespace GUI
{

    class ScreenManager
    {
        Castle::Engine *mEngine;
        ScreenPtr mConsole;
        bool mShowConsole;
        std::vector<ScreenPtr> mScreenStack;
        
    public:
        ScreenManager(Castle::Engine *engine);

        void PushScreen(ScreenPtr &&screen);

        Screen *GetCurrentScreen();

        ScreenPtr PopScreen();

        ScreenPtr RemoveScreen(Screen *screen);
        
        void SetCurrentScreen(ScreenPtr &&screen);

        void ShowConsole(bool show);
        
        bool HandleEvent(const SDL_Event &event);

        void DrawScreen(Surface &frame);
        
    };
    
} // namespace GUI

#endif
