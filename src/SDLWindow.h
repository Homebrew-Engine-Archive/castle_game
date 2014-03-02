#ifndef SDLWINDOW_H_
#define SDLWINDOW_H_

#include <stdexcept>
#include <SDL.h>

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
};

#endif
