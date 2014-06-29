#include "tgxrenderer.h"

#include <iostream>

#include <tgx/tgx.h>

namespace gmtool
{
    void TGXRenderer::RenderToStream(std::ostream &out, const core::Image &image)
    {
        tgx::WriteImage(out, image);
    }
}
