#ifndef SDLWINDOW_H_
#define SDLWINDOW_H_

#include <SDL2/SDL.h>

#include "errors.h"

struct DestoryWindowDeleter
{
    void operator()(SDL_Window *window) const {
        if(window != NULL) {
            SDL_DestroyWindow(window);
        }
    };
};

class SDLWindow
{
    mutable SDL_Window *m_wnd;
    
public:
    SDLWindow(const char *title, int width, int height);
    ~SDLWindow();

    const SDL_Window* GetWindow() const;
    SDL_Window* GetWindow();

    void Resize(int w, int h);
};

#endif
