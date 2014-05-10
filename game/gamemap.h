#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>

#include <game/camera.h>

enum class Direction;

class Surface;

namespace Castle
{
    struct TerrainTile
    {
        int heightDelta;
        int tileGroup;
        int tileIndex;
    };
    
    class GameMap
    {
        int mSize;
        std::vector<int> mTileRows;
        std::vector<int> mTileCols;
        std::vector<int> mTileHeights;
        std::vector<int> mTileGroup;
        std::vector<int> mTileIndex;
        bool mBorderless;
        
    public:
        GameMap(int size);
        
        bool Borderless() const;
        void Borderless(bool yes);

        int TileRow(int tile) const;
        int TileCol(int tile) const;
        int TileHeight(int tile) const;
        int TileGroup(int tile) const;
        int TileIndex(int tile) const;
    };

    GameMap RandomMap(uint64_t seed);
}

#endif // GAMEMAP_H_
