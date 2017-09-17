#ifndef TILEPART_H_
#define TILEPART_H_

#include <iosfwd>

namespace gmtool
{
    /// \brief It is an option that specify on what part of tile should we operate. 
    enum class TilePart : int {
        Tile,
        Box,
        Both ///< default
    };

    std::istream& operator>>(std::istream &in, TilePart &mode);
    std::ostream& operator<<(std::ostream &out, const TilePart &mode);
}

#endif // TILEPART_H_
