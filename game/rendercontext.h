#ifndef RENDERCONTEXT_H_
#define RENDERCONTEXT_H_

#include <game/rect.h>
#include <game/point.h>
#include <game/color.h>

namespace Render
{
    enum class DrawMode : int
    {
        Outline, Filled
    };

    struct RenderContext
    {
        Rect clipRectangle;
        DrawMode drawMode;
        Color drawColor;
        Point coordinateOrigin;
    };
}

#endif // RENDERCONTEXT_H_
