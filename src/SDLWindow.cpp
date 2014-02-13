#include "SDLWindow.h"

SDLWindow::SDLWindow(const char * title, int width, int height)
{
    m_wnd = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL);
    if(m_wnd == NULL)
        throw SDLError(SDL_GetError());
}

SDLWindow::~SDLWindow()
{
    SDL_DestroyWindow(m_wnd);
}

SDL_Window* SDLWindow::GetWindow()
{
    return m_wnd;
}

const SDL_Window* SDLWindow::GetWindow() const
{
    return m_wnd;
}

SDL_Rect SDLWindow::Rect() const
{
    int width, height;
    SDL_GetWindowSize(m_wnd, &width, &height);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;
    return rect;
}
