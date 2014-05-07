#include "gamemap.h"

#include <game/direction.h>
#include <random>

namespace Castle
{
    GameMap::GameMap(int size)
        : mSize(size)
    {
    }

    void GameMap::Draw(Surface &surface, int viewportX, int viewportY, Direction viewportOrient, int viewportRadius)
    {
        
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
