#include "gamemap.h"

#include <random>

#include <game/modulo.h>
#include <game/gm1.h>
#include <game/direction.h>
#include <game/landscape.h>

namespace Castle
{
    GameMap::GameMap(int size)
        : mSize(size)
        , mCellsCount(mSize * mSize)
        , mHeightLayer(mCellsCount, 0)
        , mLandscapeLayer(mCellsCount, Landscape::Land)
        , mHorizontalWrapping(false)
        , mVerticalWrapping(false)
    {
    }

    int GameMap::Size() const
    {
        return mSize;
    }

    int GameMap::Height(const Cell &cell) const
    {
        return mHeightLayer.at(CellToIndex(cell));
    }

    void GameMap::Height(const Cell &cell, int height)
    {
        mHeightLayer.at(CellToIndex(cell)) = height;
    }

    Landscape GameMap::LandscapeType(const Cell &cell) const
    {
        return mLandscapeLayer.at(CellToIndex(cell));
    }

    void GameMap::LandscapeType(const Cell &cell, Landscape land)
    {
        mLandscapeLayer.at(CellToIndex(cell)) = std::move(land);
    }
    
    int GameMap::CellToIndex(const Cell &cell) const
    {
        return cell.y * mSize + cell.x;
    }

    GameMap::Cell GameMap::IndexToCell(int index) const
    {
        return Cell(index % mSize, index / mSize);
    }
    
    bool GameMap::HasCell(const Cell &cell) const
    {
        return (CellToIndex(cell) >= 0) && (CellToIndex(cell) < mCellsCount);
    }

    void GameMap::WrapHorizontal(bool on)
    {
        mHorizontalWrapping = on;
    }

    void GameMap::WrapVertical(bool on)
    {
        mVerticalWrapping = on;
    }
    
    void GenerateRandomMap(GameMap &map)
    {
        srand((int)&map);
        
        for(int y = 0; y < map.Size(); ++y) {
            for(int x = 0; x < map.Size(); ++x) {
                const GameMap::Cell cell(x, y);
                map.LandscapeType(cell, Landscape::Land);
                map.Height(cell, rand() % 20);
            }
        }
        
        int cx = core::Mod(rand(), map.Size());
        int cy = core::Mod(rand(), map.Size());

        int dist = map.Height(GameMap::Cell(cx, cy)) + core::Mod(rand(), 20);

        std::vector<GameMap::Cell> q;
        q.emplace_back(cx, cy);

        while(q.size() != 0) {
            GameMap::Cell cell = q.back();
            q.pop_back();
            if(map.HasCell(cell)) {
                GameMap::Cell off(core::Mod(rand(), 3) - 1,
                                  core::Mod(rand(), 3) - 1);
                if(abs(cell.x - cx) + abs(cell.y - cy) < dist) {
                    q.push_back(cell + off);
                }
                map.LandscapeType(cell, Landscape::Sea);
            }
        }
    }
}
