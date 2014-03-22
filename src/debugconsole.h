#ifndef DEBUG_CONSOLE_H_
#define DEBUG_CONSOLE_H_

#include <memory>
#include "screen.h"

namespace Castle
{
    class Engine;
}

namespace GUI
{
    ScreenPtr CreateDebugConsole(Castle::Engine *engine);
}

#endif
