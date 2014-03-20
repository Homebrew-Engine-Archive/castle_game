#include "window.h"

Window::Window(const char *title, int width, int height)
{
    mWnd = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(mWnd == NULL)
        throw std::runtime_error(SDL_GetError());
}

Window::~Window()
{
    SDL_DestroyWindow(mWnd);
}

SDL_Window *Window::GetWindow()
{
    return mWnd;
}
