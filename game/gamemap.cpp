#include "gamemap.h"

#include <game/sdl_utils.h>
#include <game/direction.h>
#include <random>

namespace Castle
{
    GameMap::GameMap(int size)
        : mSize(size)
        , mBorderless(true)
    {
    }

    int GameMap::TileRow(int tile) const
    {
        return mTileRows.at(tile);
    }

    int GameMap::TileCol(int tile) const
    {
        return mTileCols.at(tile);
    }

    int GameMap::TileHeight(int tile) const
    {
        return mTileHeights.at(tile);
    }

    int GameMap::TileGroup(int tile) const
    {
        return mTileGroup.at(tile);
    }

    int GameMap::TileIndex(int tile) const
    {
        return mTileIndex.at(tile);
    }
    
    void GameMap::Borderless(bool yes)
    {
        mBorderless = yes;
    }

    bool GameMap::Borderless() const
    {
        return mBorderless;
    }
    
    GameMap RandomMap(uint64_t seed)
    {
        std::mt19937 gen(seed);
        std::uniform_int_distribution<int> dist(100, 1000);

        const int size = dist(gen);
        
        GameMap map(size);

        
        
        return map;
    }
}
