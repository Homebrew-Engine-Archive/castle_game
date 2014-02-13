#ifndef SDLRECT_H_
#define SDLRECT_H_

#include <SDL2/SDL.h>

SDL_Rect MakeRect(int x, int y, int w, int h);
SDL_Rect MakeRect(int w = 0, int h = 0);

#endif
