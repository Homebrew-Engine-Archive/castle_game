#include "tgxrenderer.h"

#include <iostream>

#include <game/tgx.h>

namespace gmtool
{
    void TGXrenderer::renderoStream(std::ostream &out, const castle::Image &image)
    {
        TGX::WriteTGX(out, image);
    }
}
