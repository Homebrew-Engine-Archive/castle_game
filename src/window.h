#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdexcept>
#include <SDL.h>
#include "sdl_utils.h"

class Window
{
    SDL_Window *mWnd;
    
public:
    Window(const char *title, int width, int height);
    ~Window();

    SDL_Window *GetWindow();
};

#endif
