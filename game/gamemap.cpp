#include "gamemap.h"

#include <random>
#include <algorithm>

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

    const GameMap::Cell GameMap::NullCell() const
    {
        return GameMap::Cell(-1, -1);
    }
    
    void GameMap::WrapHorizontal(bool on)
    {
        mHorizontalWrapping = on;
    }

    void GameMap::WrapVertical(bool on)
    {
        mVerticalWrapping = on;
    }

    const GameMap::AdjacencyIterator GameMap::AdjacencyIterator::operator++(int)
    {
        const GameMap::AdjacencyIterator tmp(*this);
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
    
    const GameMap::Cell GameMap::AdjacencyIterator::operator*() const
    {
        // staggered iso map
        if(core::Even(mCell.y)) {
            constexpr GameMap::Cell even[core::MaxDirCount] = {
                {0, 2}, {0, -2}, {-1, 0}, {1, 0},
                {-1, -1}, {-1, 1}, {0, -1}, {0, 1}
            };
            return mCell + even[mDir];
        } else {
            constexpr GameMap::Cell odd[core::MaxDirCount] = {
                {0, 2}, {0, -2}, {-1, 0}, {1, 0},
                {0, 1}, {0, -1}, {1, -1}, {1, 1}
            };
            return mCell + odd[mDir];
        }
    }
    
    const std::pair<GameMap::AdjacencyIterator, GameMap::AdjacencyIterator> GameMap::AdjacentCells(GameMap::Cell cell) const
    {
        return std::make_pair(GameMap::AdjacencyIterator(*this, cell, 0),
                              GameMap::AdjacencyIterator(*this, cell, core::MaxDirCount));
    }

    const GameMap::CellIterator GameMap::CellIterator::operator++(int)
    {
        const GameMap::CellIterator tmp(mMap, mCellIndex);
        this->operator++();
        return tmp;
    }
    
    bool GameMap::CellIterator::operator!=(const GameMap::CellIterator &that) const
    {
        return mCellIndex != that.mCellIndex;
    }

    const GameMap::Cell GameMap::CellIterator::operator*() const
    {
        return mMap.IndexToCell(mCellIndex);
    }
    
    void GameMap::CellIterator::operator++()
    {
        ++mCellIndex;
    }
    
    const std::pair<GameMap::CellIterator, GameMap::CellIterator> GameMap::Cells() const
    {
        return std::make_pair(GameMap::CellIterator(*this, 0),
                              GameMap::CellIterator(*this, mCellsCount));
    }

    void ApplyMapLandscape(GameMap &map, GameMap::Cell center, Landscape landscape)
    {
        std::set<GameMap::Cell> history;
        std::deque<GameMap::Cell> q;
        q.push_back(center);
        
        while(!q.empty()) {
            GameMap::Cell cell = q.front();
            q.pop_front();
            map.LandscapeType(cell, landscape);
            const auto xs = map.AdjacentCells(cell);
            for(auto it = xs.first; it != xs.second; ++it) {
                if(!map.HasCell(*it)) {
                    continue;
                }
                if(history.find(*it) != history.end()) {
                    continue;
                }
                if(map.Height(*it) > map.Height(cell)) {
                    continue;
                }
                history.insert(*it);
                q.push_back(*it);
            }
        }
    }
    
    void GenerateRandomMap(GameMap &map)
    {
        std::default_random_engine rnd((int)&map);
        std::uniform_int_distribution<> heightDist(0, 20);
        std::uniform_int_distribution<> landscapeDist(0, 16);
        std::uniform_int_distribution<> raindropDist(0, 100);

        std::vector<GameMap::Cell> raindrops;

        const auto cellsIters = map.Cells();
        for(auto i = cellsIters.first; i != cellsIters.second; ++i) {
            map.LandscapeType(*i, Landscape::Land);
            map.Height(*i, heightDist(rnd));
            if(raindropDist(rnd) == 0) {
                raindrops.push_back(*i);
            }
        }
        
        for(const GameMap::Cell &cell : raindrops) {
            const Landscape land = GetLandscapeByIndex(landscapeDist(rnd));
            ApplyMapLandscape(map, cell, land);
        }
    }
}
