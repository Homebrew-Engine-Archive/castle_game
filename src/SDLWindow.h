#ifndef SDLWINDOW_H_
#define SDLWINDOW_H_

#include <SDL2/SDL.h>

#include "errors.h"

class SDLWindow
{
    mutable SDL_Window *m_wnd;
    
public:
    SDLWindow(const char *title, int width, int height);
    ~SDLWindow();

    const SDL_Window* GetWindow() const;
    SDL_Window* GetWindow();

    void Resize(int w, int h);
    
    SDL_Rect Rect() const;
};

#endif
