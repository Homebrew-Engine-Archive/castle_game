#ifndef RENDERENGINE_H_
#define RENDERENGINE_H_

#include <vector>
#include <game/size.h>

class Surface;
class Color;
class Point;
class Rect;
class Line;

namespace Render
{
    enum class DrawMode : int {
        Outline, Filled
    };
    
    class RenderEngine
    {
    public:
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetOutputMode(const core::Size &size, uint32_t format) = 0;
        virtual const core::Size GetOutputSize() const = 0;
        virtual const core::Size GetMaxOutputSize() const = 0;

        virtual void DrawPoints(const Point *points, size_t count, Color const&) = 0;
        virtual void DrawRects(const Rect *rects, size_t count, Color const&, DrawMode) = 0;
        virtual void DrawLines(const Line *lines, size_t count, Color const&) = 0;
        virtual void DrawPolygon(const Point *points, size_t count, Color const&, DrawMode) = 0;
        
        virtual void DrawSurface(const Surface &image, const Rect &subrect, const Rect &screen) = 0;
    };
}

#endif // RENDERENGINE_H_
