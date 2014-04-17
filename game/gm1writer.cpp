#include "gm1writer.h"

#include <iostream>

#include <game/filesystem.h>
#include <game/endianness.h>

namespace GM1
{
    std::ostream& WriteHeader(std::ostream &out, const GM1::Header &header)
    {
        using namespace Endian;
        WriteLittle<uint32_t>(out, header.u1);
        WriteLittle<uint32_t>(out, header.u2);
        WriteLittle<uint32_t>(out, header.u3);
        WriteLittle<uint32_t>(out, header.imageCount);
        WriteLittle<uint32_t>(out, header.u4);
        WriteLittle<uint32_t>(out, header.dataClass);
        WriteLittle<uint32_t>(out, header.u5);
        WriteLittle<uint32_t>(out, header.u6);
        WriteLittle<uint32_t>(out, header.sizeCategory);
        WriteLittle<uint32_t>(out, header.u7);
        WriteLittle<uint32_t>(out, header.u8);
        WriteLittle<uint32_t>(out, header.u9);
        WriteLittle<uint32_t>(out, header.width);
        WriteLittle<uint32_t>(out, header.height);
        WriteLittle<uint32_t>(out, header.u10);
        WriteLittle<uint32_t>(out, header.u11);
        WriteLittle<uint32_t>(out, header.u12);
        WriteLittle<uint32_t>(out, header.u13);
        WriteLittle<uint32_t>(out, header.anchorX);
        WriteLittle<uint32_t>(out, header.anchorY);
        WriteLittle<uint32_t>(out, header.dataSize);
        WriteLittle<uint32_t>(out, header.u14);
        return out;
    }

    std::ostream& WritePalette(std::ostream &out, const GM1::Palette &palette)
    {
        for(GM1::PaletteEntry entry : palette) {
            Endian::WriteLittle<GM1::PaletteEntry>(out, entry);
        }
        return out;
    }
}
