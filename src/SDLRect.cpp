#include "SDLRect.h"

SDLRect::SDLRect()
    : rect(new SDL_Rect)
{
    rect->x = 0;
    rect->y = 0;
    rect->w = 0;
    rect->h = 0;
}

SDLRect::SDLRect(int w, int h)
    : rect(new SDL_Rect)
{
    rect->x = 0;
    rect->y = 0;
    rect->w = w;
    rect->h = h;
}

SDLRect::SDLRect(int x, int y, int w, int h)
    : rect(new SDL_Rect)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

SDLRect::~SDLRect()
{
    delete rect;
}

bool SDLRect::IsNull() const
{
    return (rect->w == 0 || rect->h == 0);
}

const SDL_Rect* SDLRect::GetRect() const
{
    return (IsNull() ? NULL : rect);
}

SDL_Rect* SDLRect::GetRect()
{
    return (IsNull() ? NULL : rect);
}
