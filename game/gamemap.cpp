#include "gamemap.h"
#include <game/geometry.h>

GameMap::GameMap()
{

}

GameMap::~GameMap()
{

}

GameMapLocation GameMap::GetLocationAtPoint(int, int) const
{
    GameMapLocation loc;
    return loc;
}

SDL_Rect GameMap::GetLocationRect(const GameMapLocation &) const
{
    return MakeEmptyRect();
}
