#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include "landscape.h"

struct GameMapLocation
{
    GameMapLocation()
        : x(-1), y(-1) {}
    int x;
    int y;
    bool Null() const {
        return (x < 0) || (y < 0);
    }
};

class GameMap
{
    std::vector<int> m_heightMap;
    std::vector<int> m_shadowMap;
    std::vector<LandscapeClass> m_landscape;

public:
    GameMap();
    ~GameMap();
    SDL_Rect GetLocationRect(const GameMapLocation &loc) const;
    GameMapLocation GetLocationAtPoint(int x, int y) const;
    
};

#endif
