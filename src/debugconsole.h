#ifndef DEBUG_CONSOLE_H_
#define DEBUG_CONSOLE_H_

#include <iosfwd>
#include <memory>
#include "screen.h"

namespace Castle
{
    class Engine;
}

namespace Render
{
    class Renderer;
}

namespace UI
{
    class ScreenManager;
    
    ScreenPtr CreateDebugConsole(ScreenManager *mgr, Render::Renderer *render, std::ostream &sink, std::istream &log);
}

#endif
