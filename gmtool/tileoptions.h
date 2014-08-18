#ifndef TILEOPTIONS_H_
#define TILEOPTIONS_H_

#include <iosfwd>

namespace gmtool
{
    enum class TileRenderMode {Tile, Box, Default};

    std::istream& operator>>(std::istream &in, TileRenderMode &mode);
    std::ostream& operator<<(std::ostream &out, const TileRenderMode &mode);
}

#endif // TILEOPTIONS_H_
