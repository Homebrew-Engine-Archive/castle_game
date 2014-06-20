#ifndef LANDSCAPE_H_
#define LANDSCAPE_H_

#include <game/modulo.h>

const int CliffsHeightThreshold = 20;

enum class Landscape : int {
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

class Terrain
{
    Landscape mLandscape;
public:
    explicit Terrain(Landscape type)
        : mLandscape(type)
        {}
    inline Landscape GetType() const {
        return mLandscape;
    }
};

constexpr int GetLandscapeIndex(const Landscape &landscape)
{
    return static_cast<int>(landscape);
}

constexpr Landscape GetLandscapeByIndex(int index)
{
    return static_cast<Landscape>(index);
}

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
