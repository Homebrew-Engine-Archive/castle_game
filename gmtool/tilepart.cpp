#include "tilepart.h"

#include <string>
#include <iostream>

namespace 
{
    const char *kBoth = "both";
    const char *kTile = "tile";
    const char *kBox = "box";
}

namespace gmtool
{
    std::istream& operator>>(std::istream &in, TilePart &mode)
    {
        std::string token;
        in >> token;
        if(in) {
            if(token == kBoth) {
                mode = TilePart::Both;
            } else if(token == kTile) {
                mode = TilePart::Tile;
            } else if(token == kBox) {
                mode = TilePart::Box;
            } else {
                in.clear(std::ios_base::failbit);
            }
        }
        return in;
    }

    std::ostream& operator<<(std::ostream &out, const TilePart &mode)
    {
        switch(mode) {
        case TilePart::Both:
            out << kBoth;
            break;
        case TilePart::Tile:
            out << kTile;
            break;
        case TilePart::Box:
            out << kBox;
            break;
        default:
            break;
        }
        return out;
    }
}
