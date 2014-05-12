#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>

#include <game/camera.h>
#include <game/point.h>

enum class Direction;

class Surface;

namespace Castle
{
    class GameMap
    {
        int mSize;
        std::vector<Point> mTileCoords;
        std::vector<int> mTileHeights;
        std::vector<int> mTileType;
        bool mBorderless;
        
    public:
        GameMap(int size);
        
        bool Borderless() const;
        void Borderless(bool yes);
                
        Point TileCoord(int tile) const;
        int TileHeight(int tile) const;
        int TileType(int tile) const;
    };

    GameMap RandomMap(uint64_t seed);
}

#endif // GAMEMAP_H_
