#ifndef gm1WRITER_H_
#define gm1WRITER_H_

#include <iosfwd>

namespace castle
{
    class Palette;
}

namespace gm1
{
    class Header;
}

namespace gm1
{
    std::ostream& WriteHeader(std::ostream&, gm1::Header const&);
    std::ostream& WritePalette(std::ostream&, const castle::Palette &palette);
}

#endif // gm1WRITER_H_
