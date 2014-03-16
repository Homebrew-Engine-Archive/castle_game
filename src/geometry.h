#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "SDL.h"

/**
 * The best application, i guess.
 */
SDL_Rect PutIn(const SDL_Rect &src, const SDL_Rect &dst, double x, double y);

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
SDL_Point ShiftPoint(const SDL_Point &point, int x, int y);

#endif
