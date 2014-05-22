#include "gm1writer.h"

#include <iostream>

#include <game/gm1.h>
#include <game/iohelpers.h>

namespace GM1
{
    std::ostream& WriteHeader(std::ostream &out, const GM1::Header &header)
    {
        io::WriteLittle(out, header.u1);
        io::WriteLittle(out, header.u2);
        io::WriteLittle(out, header.u3);
        io::WriteLittle(out, header.imageCount);
        io::WriteLittle(out, header.u4);
        io::WriteLittle(out, header.dataClass);
        io::WriteLittle(out, header.u5);
        io::WriteLittle(out, header.u6);
        io::WriteLittle(out, header.sizeCategory);
        io::WriteLittle(out, header.u7);
        io::WriteLittle(out, header.u8);
        io::WriteLittle(out, header.u9);
        io::WriteLittle(out, header.width);
        io::WriteLittle(out, header.height);
        io::WriteLittle(out, header.u10);
        io::WriteLittle(out, header.u11);
        io::WriteLittle(out, header.u12);
        io::WriteLittle(out, header.u13);
        io::WriteLittle(out, header.anchorX);
        io::WriteLittle(out, header.anchorY);
        io::WriteLittle(out, header.dataSize);
        io::WriteLittle(out, header.u14);
        return out;
    }

    std::ostream& WritePalette(std::ostream &out, const GM1::Palette &palette)
    {
        for(GM1::palette_entry_t entry : palette) {
            io::WriteLittle(out, entry);
        }
        return out;
    }
}
