#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <fstream>
#include <initializer_list>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <map>
#include <limits>

#include <SDL2/SDL.h>

#include "game.h"
#include "animation.h"
#include "SDLContext.h"
#include "SDLWindow.h"
#include "SDLRenderer.h"
#include "errors.h"
#include "tgx.h"
#include "gm1.h"
#include "landscape.h"

template<class OutputIterator>
void LoadStringList(const char *filename, OutputIterator out);

#endif
