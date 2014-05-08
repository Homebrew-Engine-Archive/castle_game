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
        std::vector<TerrainTile> mTerrain;
        
    public:
        GameMap(int size);
        void DrawTerrain(Surface &surface, const Castle::Camera &camera);
    };

    GameMap RandomMap(uint64_t seed);
}

#endif // GAMEMAP_H_
