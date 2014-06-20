#ifndef TGXRENDERER_H_
#define TGXRENDERER_H_

#include <gmtool/renderer.h>

namespace castle
{
    class Image;
}

namespace gmtool
{
    struct TGXRenderer : public Renderer
    {
        virtual void RenderToStream(std::ostream &out, const castle::Image &surface);
    };
}

#endif // TGXRENDERER_H_
