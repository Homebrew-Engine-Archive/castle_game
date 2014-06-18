#ifndef GM1WRITER_H_
#define GM1WRITER_H_

#include <iosfwd>

namespace Castle
{
    class Palette;
}

namespace GM1
{
    class Header;
}

namespace GM1
{
    std::ostream& WriteHeader(std::ostream&, GM1::Header const&);
    std::ostream& WritePalette(std::ostream&, const Castle::Palette &palette);
}

#endif // GM1WRITER_H_
