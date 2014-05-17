#ifndef LANDSCAPE_H_
#define LANDSCAPE_H_

const int ChevronsHeightThreshold = 20;

class TerrainClass
{
    int mId;
    int mMinHeight;
    int mMaxHeight;
    
public:
    TerrainClass(int id, int minHeight, int maxHeight)
        : mId(id)
        , mMinHeight(minHeight)
        , mMaxHeight(maxHeight)
        {}
    
    int MinHeight() const { return mMinHeight; }
    int MaxHeight() const { return mMaxHeight; }

    int Id() const { return mId; }
};

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
