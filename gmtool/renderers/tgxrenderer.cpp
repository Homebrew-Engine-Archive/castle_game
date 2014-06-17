#include "tgxrenderer.h"

#include <iostream>

#include <game/tgx.h>

namespace GMTool
{
    void TGXRenderer::RenderToStream(std::ostream &out, const Castle::Image &image)
    {
        TGX::WriteTGX(out, image);
    }
}
