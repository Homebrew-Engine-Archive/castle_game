#include "gm1writer.h"

#include <iostream>

#include <game/color.h>
#include <game/gm1palette.h>
#include <game/gm1.h>
#include <game/iohelpers.h>

namespace GM1
{
    std::ostream& WriteHeader(std::ostream &out, const GM1::Header &header)
    {
        core::WriteLittle(out, header.u1);
        core::WriteLittle(out, header.u2);
        core::WriteLittle(out, header.u3);
        core::WriteLittle(out, header.imageCount);
        core::WriteLittle(out, header.u4);
        core::WriteLittle(out, header.dataClass);
        core::WriteLittle(out, header.u5);
        core::WriteLittle(out, header.u6);
        core::WriteLittle(out, header.sizeCategory);
        core::WriteLittle(out, header.u7);
        core::WriteLittle(out, header.u8);
        core::WriteLittle(out, header.u9);
        core::WriteLittle(out, header.width);
        core::WriteLittle(out, header.height);
        core::WriteLittle(out, header.u10);
        core::WriteLittle(out, header.u11);
        core::WriteLittle(out, header.u12);
        core::WriteLittle(out, header.u13);
        core::WriteLittle(out, header.anchorX);
        core::WriteLittle(out, header.anchorY);
        core::WriteLittle(out, header.dataSize);
        core::WriteLittle(out, header.u14);
        return out;
    }

    std::ostream& WritePalette(std::ostream &out, const GM1::Palette &palette)
    {
        for(const GM1::Palette::value_type &entry : palette) {
            GM1::palette_entry_t pixel = core::ColorToPixel(entry, GM1::PalettePixelFormat);
            core::WriteLittle(out, pixel);
        }
        return out;
    }
}
