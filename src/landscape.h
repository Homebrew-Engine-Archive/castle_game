#ifndef LANDSCAPE_H_
#define LANDSCAPE_H_

#include <SDL2/SDL.h>

#include "renderer.h"

const int CHEVRONS_HEIGHT_THRESHOLD = 20;

enum class LandscapeClass : int {
    Pebbles,
    Stones,
    Rocks,
    Boulders,
    Dirt,
    Grass,                                               // zero-high
    Land,
    Iron,
    Beach,                                               // zero-high
    River,
    Ford,                                                // River
    Ripple,                                              // River
    Swamp,
    Oil,                                                 // Swamp
    Sea,                                                 // zero-high
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



#endif
