#include "gamemap.h"

#include <game/direction.h>
#include <random>

namespace Castle
{
    GameMap::GameMap(int rows, int cols)
        : mRows(rows)
        , mCols(cols)
        , mTiles(mRows * mCols, 0)
    {
        
    }

    void GameMap::Randomize(int seed)
    {
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<int> dist;
    }

    void GameMap::Draw(Surface &surface, int viewportX, int viewportY, Direction viewportOrient, int viewportRadius)
    {
        
    }
}
