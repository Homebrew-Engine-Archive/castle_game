#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>

#include <game/terrain.h>

enum class Direction;

class Surface;

namespace Castle
{
    class GameMap
    {
        int mSize;
        
    public:
        explicit GameMap(int size);
        void Draw(Surface &surface, int viewportX, int viewportY, Direction viewportOrient, int viewportRadius);
    };

    GameMap RandomMap(uint64_t seed);
}

#endif // GAMEMAP_H_
