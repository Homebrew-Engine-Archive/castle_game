#ifndef SOFTWARERENDERENGINE_H_
#define SOFTWARERENDERENGINE_H_

#include <game/sdl_utils.h>
#include <game/surface.h>
#include <game/renderengine.h>

namespace Render
{
    class SoftwareRenderEngine : public RenderEngine
    {
    protected:
        RendererPtr mScreenRenderer;
        WindowPtr mWindow;
        int mScreenWidth;
        int mScreenHeight;
        uint32_t mScreenFormat;
        Surface mScreenSurface;
        TexturePtr mScreenTexture;
        
    public:
        explicit SoftwareRenderEngine();
        virtual ~SoftwareRenderEngine();
        
        virtual void BeginFrame();
        virtual void EndFrame();

        virtual void SetOutputMode(const core::Size &size, uint32_t format);
        virtual const core::Size GetOutputSize() const;
        virtual const core::Size GetMaxOutputSize() const;

        virtual void DrawPoints(const Point *points, size_t count, const Color &color);
        virtual void DrawRects(const Rect *rects, size_t count, const Color &color, DrawMode mode);
        virtual void DrawLines(const Line *lines, size_t count, const Color &color);
        virtual void DrawPolygon(const Point *points, size_t count, const Color &color, DrawMode mode);

        virtual void DrawSurface(const Surface &image, const Rect &subrect, const Rect &screen);
    };
}

#endif // SOFTWARERENDERENGINE_H_
