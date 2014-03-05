#include "window.h"

Window::Window(const char *title, int width, int height)
{
    m_wnd = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(m_wnd == NULL)
        throw std::runtime_error(SDL_GetError());
}

Window::~Window()
{
    SDL_DestroyWindow(m_wnd);
}

SDL_Window *Window::GetWindow()
{
    return m_wnd;
}
