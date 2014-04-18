#ifndef TGXRENDERER_H_
#define TGXRENDERER_H_

#include <gmtool/renderer.h>

class Surface;

namespace GMTool
{
    struct TGXRenderer : public Renderer
    {
        virtual void RenderToStream(std::ostream &out, const Surface &surface);
    };
}

#endif // TGXRENDERER_H_
