#include "gamemap.h"

#include <algorithm>
#include <cassert>

#include <iostream>
#include <set>
#include <random>
#include <deque>

#include <game/modulo.h>
#include <game/gm1.h>
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
        return (cell.x >= 0) && (cell.y >= 0) && (cell.x < mSize) && (cell.y < mSize);
    }

    void GameMap::WrapHorizontal(bool on)
    {
        mHorizontalWrapping = on;
    }

    void GameMap::WrapVertical(bool on)
    {
        mVerticalWrapping = on;
    }

    GameMap::Cell GameMap::NullCell() const
    {
        return Cell(-1, -1);
    }

    const GameMap::AdjacencyIterator GameMap::AdjacencyIterator::operator++(int)
    {
        GameMap::AdjacencyIterator tmp(*this);
        this->operator++();
        return tmp;
    }
    
    void GameMap::AdjacencyIterator::operator++()
    {
        ++mDir;
    }

    bool GameMap::AdjacencyIterator::operator!=(const GameMap::AdjacencyIterator &that) const
    {
        return mDir != that.mDir;
    }
    
    GameMap::Cell GameMap::AdjacencyIterator::operator*() const
    {
        // stuggered iso map
        constexpr GameMap::Cell adj[MaxDirCount] = {
            {-1, -1}, {-1, 1}, {1, 1}, {1, -1},
            {0, 1}, {0, -1}, {1, 0}, {-1, 0}
        };
        return mCell + adj[mDir];
    }
    
    std::pair<GameMap::AdjacencyIterator, GameMap::AdjacencyIterator> GameMap::AdjacentCells(GameMap::Cell cell) const
    {
        return std::make_pair(GameMap::AdjacencyIterator(*this, cell, 0),
                              GameMap::AdjacencyIterator(*this, cell, MaxDirCount));
    }
        
    void GenerateRandomMap(GameMap &map)
    {
        srand((int)&map);

        const auto size = map.Size();
        for(int y = 0; y < size; ++y) {
            for(int x = 0; x < size; ++x) {
                const GameMap::Cell cell(x, y);
                map.LandscapeType(cell, Landscape::Land);
                map.Height(cell, rand() % 20);
            }
        }

        GameMap::Cell center(core::Mod(rand(), map.Size()),
                             core::Mod(rand(), map.Size()));

        std::set<GameMap::Cell> history;
        std::deque<GameMap::Cell> q;
        q.push_back(center);
        
        while(!q.empty()) {
            GameMap::Cell cell = q.front();
            q.pop_front();
            int dist = Hypot(cell, center);
            map.LandscapeType(cell, GetLandscapeByIndex(dist));
            const auto xs = map.AdjacentCells(cell);
            for(auto it = xs.first; it != xs.second; ++it) {
                if(map.HasCell(*it) && history.find(*it) == history.end()) {
                    history.insert(*it);
                    q.push_back(*it);
                }
            }
        }
    }
}
