#include "gm1writer.h"

#include <iostream>

#include <core/iohelpers.h>
#include <core/color.h>
#include <game/palette.h>
#include <game/gm1.h>

namespace gm1
{
    std::ostream& WriteHeader(std::ostream &out, const gm1::Header &header)
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

    std::ostream& WritePalette(std::ostream &out, const castle::Palette &palette)
    {
        for(const castle::Palette::value_type &entry : palette) {
            const core::Color color(entry);
            const palette_entry_t pixel = color.ConvertTo(PalettePixelFormat);
            core::WriteLittle(out, pixel);
        }
        return out;
    }
}
