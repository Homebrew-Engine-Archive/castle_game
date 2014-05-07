#ifndef GM1WRITER_H_
#define GM1WRITER_H_

#include <game/gm1palette.h>

namespace GM1
{
    class Header;
}

namespace GM1
{
    std::ostream& WriteHeader(std::ostream&, GM1::Header const&);
    std::ostream& WritePalette(std::ostream&, GM1::Palette const&);
}

#endif // GM1WRITER_H_
