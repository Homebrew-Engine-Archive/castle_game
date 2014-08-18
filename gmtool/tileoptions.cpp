#include "tileoptions.h"

#include <string>
#include <iostream>

namespace 
{
    const char *kDefault = "default";
    const char *kTile = "tile";
    const char *kBox = "box";
}

namespace gmtool
{
    std::istream& operator>>(std::istream &in, TileRenderMode &mode)
    {
        std::string token;
        in >> token;
        if(token == kDefault) {
            mode = TileRenderMode::Default;
        } else if(token == kTile) {
            mode = TileRenderMode::Tile;
        } else if(token == kBox) {
            mode = TileRenderMode::Box;
        } else {
            in.clear(std::ios_base::failbit);
        }
        return in;
    }

    std::ostream& operator<<(std::ostream &out, const TileRenderMode &mode)
    {
        switch(mode) {
        case TileRenderMode::Default:
            out << "default";
            break;
        case TileRenderMode::Tile:
            out << "tile";
            break;
        case TileRenderMode::Box:
            out << "box";
            break;
        default:
            break;
        }
        return out;
    }
}
