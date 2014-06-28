#ifndef TGXRENDERER_H_
#define TGXRENDERER_H_

#include <gmtool/renderer.h>

namespace core
{
    class Image;
}

namespace gmtool
{
    struct TGXRenderer : public Renderer
    {
        virtual void RenderToStream(std::ostream &out, const core::Image &surface);
    };
}

#endif // TGXRENDERER_H_
