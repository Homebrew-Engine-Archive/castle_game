#ifndef TGXRENDERER_H_
#define TGXRENDERER_H_

#include <gmtool/renderer.h>

namespace Castle
{
    class Image;
}

namespace GMTool
{
    struct TGXRenderer : public Renderer
    {
        virtual void RenderToStream(std::ostream &out, const Castle::Image &surface);
    };
}

#endif // TGXRENDERER_H_
