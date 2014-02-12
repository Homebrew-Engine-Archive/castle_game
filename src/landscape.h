#ifndef LANDSCAPE_H_
#define LANDSCAPE_H_

#include <SDL2/SDL.h>

#include "SDLRenderer.h"

enum class LandscapeClass : int {
    Pebbles,
    Stones,
    Rocks,
    Boulders,
    Dirt,
    Grass,
    Land,
    Iron,
    Beach,
    River,
    Ford,                                                // River
    Ripple,                                              // River
    Swamp,
    Oil,                                                 // Swamp
    Sea,
    Mash                                                 // Sea
};

enum class RockClass : int {
    LargeRock,                                           // 4x4
    StandardRock,                                        // 3x3
    SmallRock,                                           // 2x2
    TinyRock                                             // 1x1
};

enum class TreeClass : int {
    Apple,
    Oak,
    Pine,
    Chestnut,
    Birch,
    Shrub
};

struct Tile
{
    Tile(LandscapeClass land, int height);
    void Draw(SDLRenderer &renderer) const;
    int height;
    LandscapeClass landscape;
};

#endif
