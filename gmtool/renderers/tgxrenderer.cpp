#include "tgxrenderer.h"

#include <iostream>

#include <game/tgx.h>

namespace GMTool
{
    void TGXRenderer::RenderToStream(std::ostream &out, const Surface &surface)
    {
        TGX::WriteTGX(out, surface);
    }
}
