#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <SDL.h>

#include <game/surface.h>

namespace Castle
{
    class Terrain
    {
    public:
        bool Stackable(const Terrain&) const;
    };
}

#endif // TERRAIN_H_
