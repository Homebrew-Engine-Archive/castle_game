#ifndef WINDOW_H_
#define WINDOW_H_

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

class Window
{
    SDL_Window *m_wnd;
    
public:
    Window(const char *title, int width, int height);
    ~Window();

    SDL_Window *GetWindow();
};

#endif