#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>

#include "macrosota.h"
#include "SDL.h"

SDL_Rect AlignRect(const SDL_Rect &src, const SDL_Rect &dst, double x, double y);
SDL_Rect MakeRect(int x, int y, int w, int h);
SDL_Rect MakeRect(int w, int h);
SDL_Rect MakeEmptyRect();
SDL_Rect PadIn(const SDL_Rect &src, int pad);
SDL_Rect ShiftRect(const SDL_Rect &src, int xshift, int yshift);
bool IsInRect(const SDL_Rect &rect, int x, int y);
SDL_Point MakePoint(int x, int y);
SDL_Point TopRight(const SDL_Rect &src);
SDL_Point BottomLeft(const SDL_Rect &src);
SDL_Point TopLeft(const SDL_Rect &src);
SDL_Point BottomRight(const SDL_Rect &src);
SDL_Point AlignPoint(const SDL_Rect &dst, double x, double y);
SDL_Rect MakeRect(const SDL_Point &p1, const SDL_Point &p2);
SDL_Rect MakeRect(const SDL_Point &p);

std::ostream &operator<<(std::ostream &out, const SDL_Rect &rect);

#endif
