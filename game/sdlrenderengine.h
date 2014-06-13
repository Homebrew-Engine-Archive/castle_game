#ifndef SDLRENDERENGINE_H_
#define SDLRENDERENGINE_H_

#include <game/rect.h>
#include <game/outputmode.h>
#include <game/sdl_utils.h>
#include <game/renderengine.h>

namespace Render
{
    class SDLRenderEngine : public RenderEngine
    {
    protected:
        RendererPtr mRenderer;
        WindowPtr mWindow;
        OutputMode mOutputMode;
        Rect mClipRect;
        int mOpacityMod;
        
    public:
        explicit SDLRenderEngine();
        virtual ~SDLRenderEngine();

        virtual void BeginFrame();
        virtual void EndFrame();

        virtual void SetOutputMode(const OutputMode &mode);
        virtual const OutputMode GetOutputMode() const;
        virtual const core::Size GetMaxOutputSize() const;

        virtual void DrawPoints(const Point *points, size_t count, Color const&);
        virtual void DrawRects(const Rect *rects, size_t count, Color const&, DrawMode);
        virtual void DrawLines(const core::Line *lines, size_t count, Color const&);
        virtual void DrawPolygon(const Point *points, size_t count, Color const&, DrawMode);
        virtual void DrawSurface(const Surface &image, const Rect &subrect, const Rect &screen);
        virtual void DrawSurfaceTiled(const Surface &image, const Rect &source, const Rect &target);
        virtual void DrawSurfaceScaled(const Surface &image, const Rect &source, const Rect &target);

        virtual void ClipRect(const Rect &rect);
        virtual void SetOpacityMod(int opacity);
        virtual void ClearOutput(const Color &color);
    private:
        void UpdateClipRect(const Rect &rect);
        void UpdateDrawColor(const Color &color);
    };
}

#endif // SDLRENDERENGINE_H_
