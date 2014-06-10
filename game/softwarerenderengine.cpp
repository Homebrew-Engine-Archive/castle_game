#include "softwarerenderengine.h"

namespace Render
{
    void SoftwareRenderEngine::BeginFrame()
    {
    }
    
    void SoftwareRenderEngine::EndFrame()
    {
    }
    
    void SoftwareRenderEngine::SetOutputMode(const core::Size &size, uint32_t format)
    {
    }
    
    const core::Size SoftwareRenderEngine::GetOutputSize() const
    {
        return core::Size();
    }
    
    const core::Size SoftwareRenderEngine::GetMaxOutputSize() const
    {
        return core::Size();
    }
    
    void SoftwareRenderEngine::DrawPoints(const Point *points, size_t count, const Color &color)
    {
    }
    
    void SoftwareRenderEngine::DrawRects(const Rect *rects, size_t count, const Color &color, DrawMode mode)
    {
    }
    
    void SoftwareRenderEngine::DrawLines(const Line *lines, size_t count, const Color &color)
    {
    }
    
    void SoftwareRenderEngine::DrawPolygon(const Point *points, size_t count, const Color &color, DrawMode mode)
    {
    }    

    void SoftwareRenderEngine::DrawSurface(const Surface &image, const Rect &subrect, const Rect &screen)
    {
    }
    
}
