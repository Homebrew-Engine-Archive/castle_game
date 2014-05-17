#ifndef IRENDERER_H_
#define IRENDERER_H_

#include <game/rect.h>

class Surface;

namespace Render
{
    struct IRenderer
    {
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetScreenSize(int width, int height) = 0;
        virtual Rect GetScreenSize() const = 0;

        virtual void PaintText(const std::string &text) = 0;
        virtual void PaintSurface(const class Surface &surface, const Rect &whither) = 0;
    };
}

#endif // IRENDERER_H_
