#include <iostream>
#include <SDL2/SDL.h>

#include "surface.h"

int main()
{
    SDL_Rect h = MakeRect(0, 0, 100, 1);
    SDL_Rect g = MakeRect(0, 0, 10, 1);
    SDL_Rect r = AlignRect(h, g, -1.0f, 0);
    PrintRect(r);
    return 0;
}
