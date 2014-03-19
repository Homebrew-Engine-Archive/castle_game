#ifndef DEBUG_CONSOLE_H_
#define DEBUG_CONSOLE_H_

#include "SDL.h"
#include "rootscreen.h"
#include "screen.h"
#include "surface.h"

class DebugConsole : public Screen
{
private:
    struct DebugConsolePimpl *m;
    
public:
    DebugConsole(RootScreen *root);
    DebugConsole(const DebugConsole &) = delete;
    DebugConsole(DebugConsole &&) = default;
    DebugConsole &operator=(const DebugConsole &) = delete;
    DebugConsole &operator=(DebugConsole &&) = default;
    ~DebugConsole();
    
    void Draw(Surface &frame);
    bool HandleEvent(const SDL_Event &event);
};

#endif
