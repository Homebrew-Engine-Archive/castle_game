#include "debugconsole.h"
#include "geometry.h"

struct DebugConsolePimpl
{
    RootScreen *root;
    DebugConsolePimpl(RootScreen *root);
};

DebugConsolePimpl::DebugConsolePimpl(RootScreen *root)
    : root(root)
{ }

DebugConsole::DebugConsole(RootScreen *root)
    : m(new DebugConsolePimpl(root))
{ }

DebugConsole::~DebugConsole()
{
    delete m;
}

void DebugConsole::Draw(Surface &frame)
{
    SDL_Rect bounds = SurfaceBounds(frame);
    SDL_Rect tophalf = MakeRect(bounds.w, bounds.h / 2);

    FillFrame(frame, &tophalf, 0x7f000000);
}

bool DebugConsole::HandleEvent(const SDL_Event &event)
{
    return false;
    switch(event.type) {
    case SDL_QUIT:
        return false;
    default:
        return true;
    }
}
