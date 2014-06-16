#ifndef SDLRENDERENGINE_H_
#define SDLRENDERENGINE_H_

#include <game/rect.h>
#include <game/outputmode.h>
#include <game/sdl_utils.h>
#include <game/renderengine.h>

namespace core
{
    class Color;
    class Point;
}

namespace Render
{
    class SDLRenderEngine : public RenderEngine
    {
    protected:
        RendererPtr mRenderer;
        WindowPtr mWindow;
        OutputMode mOutputMode;
        core::Rect mClipRect;
        int mOpacityMod;
        
    public:
        explicit SDLRenderEngine();
        virtual ~SDLRenderEngine();

        virtual void BeginFrame();
        virtual void EndFrame();

        virtual void SetOutputMode(const OutputMode &mode);
        virtual const OutputMode GetOutputMode() const;
        virtual const core::Size GetMaxOutputSize() const;

        virtual void DrawPoints(const core::Point *points, size_t count, core::Color const&);
        virtual void DrawRects(const core::Rect *rects, size_t count, core::Color const&, DrawMode);
        virtual void DrawLines(const core::Line *lines, size_t count, core::Color const&);
        virtual void DrawPolygon(const core::Point *points, size_t count, core::Color const&, DrawMode);
        virtual void DrawSurface(const Surface &image, const core::Rect &subrect, const core::Rect &screen);
        virtual void DrawSurfaceTiled(const Surface &image, const core::Rect &source, const core::Rect &target);
        virtual void DrawSurfaceScaled(const Surface &image, const core::Rect &source, const core::Rect &target);

        virtual void ClipRect(const core::Rect &rect);
        virtual void SetOpacityMod(int opacity);
        virtual void ClearOutput(const core::Color &color);
        
    private:
        void UpdateClipRect(const core::Rect &rect);
        void UpdateDrawColor(const core::Color &color);
    };
}

#endif // SDLRENDERENGINE_H_
