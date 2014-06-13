#ifndef SOFTWARERENDERENGINE_H_
#define SOFTWARERENDERENGINE_H_

#include <game/texture.h>
#include <game/rect.h>
#include <game/outputmode.h>
#include <game/sdl_utils.h>
#include <game/surface.h>
#include <game/renderengine.h>

namespace core
{
    class Size;
    class Line;
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

        Rect mClipRect;
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
        virtual void DrawPoints(const Point *points, size_t count, const Color &color);
        virtual void DrawRects(const Rect *rects, size_t count, const Color &color, DrawMode mode);
        virtual void DrawLines(const core::Line *lines, size_t count, const Color &color);
        virtual void DrawPolygon(const Point *points, size_t count, const Color &color, DrawMode mode);
        virtual void DrawSurface(const Surface &image, const Rect &subrect, const Rect &screen);
        virtual void DrawSurfaceTiled(const Surface &image, const Rect &source, const Rect &target);
        virtual void DrawSurfaceScaled(const Surface &image, const Rect &source, const Rect &target);
        virtual void SetOpacityMod(int opacity);
        virtual void ClipRect(const Rect &rect);
        virtual void ClearOutput(const Color &color);

    private:
        bool ReallocationRequired(const OutputMode &mode) const;
        void UpdateDrawColor(const Color &color);
        void UpdateClipRect(const Rect &clipRect);
    };
}

#endif // SOFTWARERENDERENGINE_H_
