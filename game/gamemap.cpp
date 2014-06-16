#include "gamemap.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <random>
#include <set>

#include <game/modulo.h>
#include <game/landscape.h>

namespace Castle
{
    namespace World
    {
        Map::Map(const Map &map) = default;
        Map& Map::operator=(Map const&) = default;
        Map::Map(Map&&) = default;
        Map& Map::operator=(Map&&) = default;
        Map::~Map() = default;
    
        Map::Map(int size)
            : mSize(size)
            , mCellsCount(mSize * mSize)
            , mHeightLayer(mCellsCount, 0)
            , mLandscapeLayer(mCellsCount, Landscape::Land)
            , mHorizontalWrapping(false)
            , mVerticalWrapping(false)
        {
        }

        int Map::Size() const
        {
            return mSize;
        }

        int Map::Height(const Cell &cell) const
        {
            return mHeightLayer.at(CellToIndex(cell));
        }

        void Map::Height(const Cell &cell, int height)
        {
            mHeightLayer.at(CellToIndex(cell)) = height;
        }

        Landscape Map::LandscapeType(const Cell &cell) const
        {
            return mLandscapeLayer.at(CellToIndex(cell));
        }

        void Map::LandscapeType(const Cell &cell, Landscape land)
        {
            mLandscapeLayer.at(CellToIndex(cell)) = std::move(land);
        }
    
        int Map::CellToIndex(const Cell &cell) const
        {
            return cell.y * mSize + cell.x;
        }

        const Map::Cell Map::IndexToCell(int index) const
        {
            return Cell(core::Mod(index, mSize), index / mSize);
        }
    
        bool Map::HasCell(const Cell &cell) const
        {
            return (cell.x >= 0) && (cell.y >= 0) && (cell.x < mSize) && (cell.y < mSize);
        }

        const Map::Cell Map::NullCell() const
        {
            return Map::Cell(-1, -1);
        }
    
        void Map::WrapHorizontal(bool on)
        {
            mHorizontalWrapping = on;
        }

        void Map::WrapVertical(bool on)
        {
            mVerticalWrapping = on;
        }

        const Map::AdjacencyIterator Map::AdjacencyIterator::operator++(int)
        {
            const Map::AdjacencyIterator tmp(*this);
            this->operator++();
            return tmp;
        }
    
        void Map::AdjacencyIterator::operator++()
        {
            ++mDir;
        }

        bool Map::AdjacencyIterator::operator!=(const Map::AdjacencyIterator &that) const
        {
            return mDir != that.mDir;
        }
    
        const Map::Cell Map::AdjacencyIterator::operator*() const
        {
            // staggered iso map
            if(core::Even(mCell.y)) {
                constexpr Map::Cell even[core::MaxDirCount] = {
                    {0, 2}, {0, -2}, {-1, 0}, {1, 0},
                    {-1, -1}, {-1, 1}, {0, -1}, {0, 1}
                };
                return mCell + even[mDir];
            } else {
                constexpr Map::Cell odd[core::MaxDirCount] = {
                    {0, 2}, {0, -2}, {-1, 0}, {1, 0},
                    {0, 1}, {0, -1}, {1, -1}, {1, 1}
                };
                return mCell + odd[mDir];
            }
        }
    
        const std::pair<Map::AdjacencyIterator, Map::AdjacencyIterator> Map::AdjacentCells(Map::Cell cell) const
        {
            return std::make_pair(Map::AdjacencyIterator(*this, cell, 0),
                                  Map::AdjacencyIterator(*this, cell, core::MaxDirCount));
        }

        const Map::CellIterator Map::CellIterator::operator++(int)
        {
            const Map::CellIterator tmp(mMap, mCellIndex);
            this->operator++();
            return tmp;
        }
    
        bool Map::CellIterator::operator!=(const Map::CellIterator &that) const
        {
            return mCellIndex != that.mCellIndex;
        }

        const Map::Cell Map::CellIterator::operator*() const
        {
            return mMap.IndexToCell(mCellIndex);
        }
    
        void Map::CellIterator::operator++()
        {
            ++mCellIndex;
        }
    
        const std::pair<Map::CellIterator, Map::CellIterator> Map::Cells() const
        {
            return std::make_pair(Map::CellIterator(*this, 0),
                                  Map::CellIterator(*this, mCellsCount));
        }

        void ApplyMapLandscape(Map &map, Map::Cell center, Landscape landscape)
        {
            std::set<Map::Cell> history;
            std::deque<Map::Cell> q;
            q.push_back(center);
        
            while(!q.empty()) {
                Map::Cell cell = q.front();
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
    
        void GenerateRandomMap(Map &map)
        {
            std::default_random_engine rnd((int)&map);
            std::uniform_int_distribution<> heightDist(0, 20);
            std::uniform_int_distribution<> landscapeDist(0, 16);
            std::uniform_int_distribution<> raindropDist(0, 100);

            std::vector<Map::Cell> raindrops;

            const auto cellsIters = map.Cells();
            for(auto i = cellsIters.first; i != cellsIters.second; ++i) {
                map.LandscapeType(*i, Landscape::Land);
                map.Height(*i, heightDist(rnd));
                if(raindropDist(rnd) == 0) {
                    raindrops.push_back(*i);
                }
            }
        
            for(const Map::Cell &cell : raindrops) {
                const Landscape land = GetLandscapeByIndex(landscapeDist(rnd));
                ApplyMapLandscape(map, cell, land);
            }
        }
    }
}
