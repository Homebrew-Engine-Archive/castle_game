#include "gamemap.h"

#include <random>

#include <game/gm1.h>
#include <game/sdl_utils.h>
#include <game/direction.h>
#include <game/landscape.h>
#include <game/surface.h>

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

    int GameMap::Rows() const
    {
        return mSize;
    }

    int GameMap::Cols() const
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
        int cy = rand() % map.Rows();
        int cx = rand() % map.Cols();

        int maxHeight = map.Rows() + map.Cols();
        
        for(int i = 0; i < map.Rows(); ++i) {
            for(int j = 0; j < map.Cols(); ++j) {
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
