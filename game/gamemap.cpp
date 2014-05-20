#include "gamemap.h"

#include <random>

#include <game/gm1.h>
#include <game/direction.h>
#include <game/landscape.h>

namespace Castle
{
    GameMap::GameMap(int size)
        : mSize(size)
        , mCells(size * size, Cell(Landscape::Land, 0))
        , mBorderless(true)
    {
    }
    
    void GameMap::Borderless(bool yes)
    {
        mBorderless = yes;
    }

    bool GameMap::Borderless() const
    {
        return mBorderless;
    }

    int GameMap::Size() const
    {
        return mSize;
    }

    Cell const& GameMap::GetCell(int row, int col) const
    {
        return mCells.at(row * mSize + col);
    }
    
    Cell& GameMap::GetCell(int row, int col)
    {
        return mCells.at(row * mSize + col);
    }

    void GenerateRandomMap(GameMap &map)
    {
        srand((int)&map);
        
        int cy = rand() % map.Size();
        int cx = rand() % map.Size();

        int maxHeight = 2 * map.Size();
        
        for(int i = 0; i < map.Size(); ++i) {
            for(int j = 0; j < map.Size(); ++j) {
                Cell &cell = map.GetCell(i, j);
                cell.Height(maxHeight - sqrt((cy-i)*(cy-i) + (cx-j)*(cx-j)));
            }
        }
    }
}

namespace Castle
{
    Cell::Cell(Landscape land, int height)
        : mHeight(height)
        , mLandscape(land)
    {
    }
    
    int Cell::Height() const
    {
        return mHeight;
    }
    
    void Cell::Height(int height)
    {
        mHeight = height;
    }
}
