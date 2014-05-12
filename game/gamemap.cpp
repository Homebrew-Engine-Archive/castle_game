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

    Point GameMap::TileCoord(int tile) const
    {
        return mTileCoords.at(tile);
    }

    int GameMap::TileHeight(int tile) const
    {
        return mTileHeights.at(tile);
    }

    int GameMap::TileType(int tile) const
    {
        return mTileType.at(tile);
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
