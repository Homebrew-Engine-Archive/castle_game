#ifndef RENDERENGINE_H_
#define RENDERENGINE_H_

#include <exception>

namespace core
{
    class Point;
    class Line;
    class Size;
    class Rect;
    class Color;
    class Image;
}

namespace castle
{
    namespace render
    {
        class OutputMode;
    }
    
    namespace render
    {
        enum class DrawMode : int
        {
            Outline, Filled
        };
    
        struct draw_mode_error : public virtual std::exception
        {
            inline const char* what() const throw() {
                return "wrong draw mode";
            }
        };
    
        class RenderEngine
        {
        public:
            virtual void BeginFrame() = 0;
            virtual void EndFrame() = 0;

            virtual void SetOutputMode(const OutputMode &mode) = 0;
            virtual const OutputMode GetOutputMode() const = 0;
            virtual const core::Size GetMaxOutputSize() const = 0;

            virtual void DrawPoints(const core::Point *points, size_t count, const core::Color &color) = 0;
            virtual void DrawRects(const core::Rect *rects, size_t count, const core::Color &color, DrawMode mode) = 0;
            virtual void DrawLines(const core::Line *lines, size_t count, const core::Color &color) = 0;
            virtual void DrawPolygon(const core::Point *points, size_t count, const core::Color &color, DrawMode mode) = 0;
            virtual void DrawImage(const core::Image &image, const core::Rect &source, const core::Point &target) = 0;
            virtual void DrawImageTiled(const core::Image &image, const core::Rect &source, const core::Rect &target) = 0;
            virtual void DrawImageScaled(const core::Image &image, const core::Rect &source, const core::Rect &target) = 0;

            virtual void SetViewport(const core::Rect &rect) = 0;
            virtual void SetOpacityMod(int opacity) = 0;

            virtual void ClearOutput(const core::Color &color) = 0;
        };
    }
}

#endif // RENDERENGINE_H_
