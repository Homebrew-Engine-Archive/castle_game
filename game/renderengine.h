#ifndef RENDERENGINE_H_
#define RENDERENGINE_H_

#include <stdexcept>

class RenderContext;
class OutputMode;
class Surface;
class Color;
class Point;
class Rect;

namespace core
{
    class Line;
    class Size;
}

namespace Render
{
    class Texture;
}

namespace Render
{
    enum class DrawMode : int
    {
        Outline, Filled
    };
    
    struct wrong_draw_mode : public std::runtime_error
    {
        explicit wrong_draw_mode() throw()
            : std::runtime_error("wrong draw mode")
            {}
    };
    
    class RenderEngine
    {
    public:
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetOutputMode(const OutputMode &mode) = 0;
        virtual const OutputMode GetOutputMode() const = 0;
        virtual const core::Size GetMaxOutputSize() const = 0;

        virtual void DrawPoints(const Point *points, size_t count, const Color &color) = 0;
        virtual void DrawRects(const Rect *rects, size_t count, const Color &color, DrawMode mode) = 0;
        virtual void DrawLines(const core::Line *lines, size_t count, const Color &color) = 0;
        virtual void DrawPolygon(const Point *points, size_t count, const Color &color, DrawMode mode) = 0;
        virtual void DrawSurface(const Surface &image, const Rect &source, const Rect &target) = 0;
        virtual void DrawSurfaceTiled(const Surface &image, const Rect &source, const Rect &target) = 0;
        virtual void DrawSurfaceScaled(const Surface &image, const Rect &source, const Rect &target) = 0;

        virtual void ClipRect(const Rect &rect) = 0;
        virtual void SetOpacityMod(int opacity) = 0;

        virtual void ClearOutput(const Color &color) = 0;
    };

}
#endif // RENDERENGINE_H_
