#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <SDL.h>

#include <game/surface.h>
#include <game/collection.h>

namespace Castle
{
    class Terrain
    {
    public:
        void Draw(Surface &surface, int x, int y);
    };
}

#endif // TERRAIN_H_
