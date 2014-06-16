#ifndef SOFTWARERENDERENGINE_H_
#define SOFTWARERENDERENGINE_H_

#include <game/rect.h>
#include <game/outputmode.h>
#include <game/sdl_utils.h>
#include <game/surface.h>
#include <game/renderengine.h>

namespace core
{
    class Size;
    class Line;
    class Point;
}

namespace Render
{
    class SoftwareRenderEngine : public RenderEngine
    {
        RendererPtr mScreenRenderer;
        WindowPtr mWindow;

        /** mOutputMode is not yet active output mode but pending out
            and mFrameOutputMode is current frame's mode
         **/
        OutputMode mOutputMode;
        OutputMode mFrameOutputMode;

        core::Rect mClipRect;
        Surface mScreenSurface;
        TexturePtr mScreenTexture;
        RendererPtr mPrimitiveRenderer;
        int mOpacityMod;
        
    public:
        explicit SoftwareRenderEngine();
        virtual ~SoftwareRenderEngine();
        
        virtual void BeginFrame();
        virtual void EndFrame();
        virtual void SetOutputMode(const OutputMode &mode);
        virtual const OutputMode GetOutputMode() const;
        virtual const core::Size GetMaxOutputSize() const;
        virtual void DrawPoints(const core::Point *points, size_t count, const core::Color &color);
        virtual void DrawRects(const core::Rect *rects, size_t count, const core::Color &color, DrawMode mode);
        virtual void DrawLines(const core::Line *lines, size_t count, const core::Color &color);
        virtual void DrawPolygon(const core::Point *points, size_t count, const core::Color &color, DrawMode mode);
        virtual void DrawSurface(const Surface &image, const core::Rect &subrect, const core::Rect &screen);
        virtual void DrawSurfaceTiled(const Surface &image, const core::Rect &source, const core::Rect &target);
        virtual void DrawSurfaceScaled(const Surface &image, const core::Rect &source, const core::Rect &target);
        virtual void SetOpacityMod(int opacity);
        virtual void ClipRect(const core::Rect &rect);
        virtual void ClearOutput(const core::Color &color);

    private:
        bool ReallocationRequired(const OutputMode &mode) const;
        void UpdateDrawColor(const core::Color &color);
        void UpdateClipRect(const core::Rect &clipRect);
    };
}

#endif // SOFTWARERENDERENGINE_H_
