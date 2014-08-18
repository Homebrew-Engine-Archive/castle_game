#include "tgximagewriter.h"

#include <iostream>
#include <tgx/tgx.h>

namespace gmtool
{
    void TGXImageWriter::Write(std::ostream &out, const core::Image &image) const
    {
        tgx::WriteImage(out, image);
    }
}
