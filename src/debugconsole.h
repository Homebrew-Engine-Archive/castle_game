#ifndef DEBUG_CONSOLE_H_
#define DEBUG_CONSOLE_H_

#include <memory>
#include "screen.h"

class RootScreen;

ScreenPtr CreateDebugConsole(RootScreen *root);

#endif
