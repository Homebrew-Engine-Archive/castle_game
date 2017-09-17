#include "gamemap.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <random>
#include <set>

#include <core/point.h>
#include <core/modulo.h>
#include <game/landscape.h>

namespace castle
{
    namespace world
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

        int Map::Height(const MapCell &cell) const
        {
            return mHeightLayer.at(CellToIndex(cell));
        }

        void Map::Height(const MapCell &cell, int height)
        {
            mHeightLayer.at(CellToIndex(cell)) = height;
        }

        Landscape Map::LandscapeType(const MapCell &cell) const
        {
            return mLandscapeLayer.at(CellToIndex(cell));
        }

        void Map::LandscapeType(const MapCell &cell, Landscape land)
        {
            mLandscapeLayer.at(CellToIndex(cell)) = std::move(land);
        }
    
        int Map::CellToIndex(const MapCell &cell) const
        {
            return cell.Y() * mSize + cell.X();
        }

        const MapCell Map::IndexToCell(int index) const
        {
            return MapCell(core::Mod(index, mSize), index / mSize);
        }
    
        bool Map::HasCell(const MapCell &cell) const
        {
            return (cell.X() >= 0) && (cell.Y() >= 0) && (cell.X() < mSize) && (cell.Y() < mSize);
        }

        const MapCell Map::NullCell() const
        {
            return MapCell(-1, -1);
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
    
        const MapCell Map::AdjacencyIterator::operator*() const
        {
            // staggered iso map
            if(core::Even(mCell.Y())) {
                constexpr MapCell even[core::MaxDirCount] = {
                    {0, 2}, {0, -2}, {-1, 0}, {1, 0},
                    {-1, -1}, {-1, 1}, {0, -1}, {0, 1}
                };
                return mCell + even[mDir];
            } else {
                constexpr MapCell odd[core::MaxDirCount] = {
                    {0, 2}, {0, -2}, {-1, 0}, {1, 0},
                    {0, 1}, {0, -1}, {1, -1}, {1, 1}
                };
                return mCell + odd[mDir];
            }
        }
    
        const std::pair<Map::AdjacencyIterator, Map::AdjacencyIterator> Map::AdjacentCells(const MapCell &cell) const
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

        const MapCell Map::CellIterator::operator*() const
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

        void ApplyMapLandscape(Map &map, const MapCell &center, Landscape landscape)
        {
            std::set<MapCell> history;
            std::deque<MapCell> q;
            q.push_back(center);
        
            while(!q.empty()) {
                const MapCell cell = q.front();
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
    
        void GenerateTestMap(Map &map)
        {
            std::default_random_engine rnd((std::size_t)(&map));
            std::uniform_int_distribution<> heightDist(0, 20);
            std::uniform_int_distribution<> landscapeDist(0, 16);
            std::uniform_int_distribution<> raindropDist(0, 100);

            std::vector<MapCell> raindrops;

            const auto cellsIters = map.Cells();
            for(auto i = cellsIters.first; i != cellsIters.second; ++i) {
                map.LandscapeType(*i, Landscape::Land);
                map.Height(*i, heightDist(rnd));
                if(raindropDist(rnd) == 0) {
                    raindrops.push_back(*i);
                }
            }
        
            for(const MapCell &cell : raindrops) {
                const Landscape land = GetLandscapeByIndex(landscapeDist(rnd));
                ApplyMapLandscape(map, cell, land);
            }
        }
    }
}
