#ifndef SURFACE_DRAWING_H_
#define SURFACE_DRAWING_H_

#include <SDL.h>

class Surface;
class Rect;
class Color;

namespace Render
{
    void DrawRhombus(SDL_Renderer &renderer, const Rect &bounds, const Color &color);
    void DrawFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color);
    void FillFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color);
    void FillRhombus(SDL_Renderer &renderer, const Rect &bounds, const Color &color);
}

namespace Graphics
{
    void DrawRhombus(Surface &dst, const Rect &bounds, const Color &color);
    void DrawFrame(Surface &dst, const Rect &frame, const Color &color);
    void FillFrame(Surface &dst, const Rect &frame, const Color &color);
    void FillRhombus(Surface &dst, const Rect &bounds, const Color &color);
}

#endif // SURFACE_DRAWING_H_
