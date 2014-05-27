#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>
#include <iterator>

#include <game/point.h>
#include <game/direction.h>

enum class Landscape;

namespace Castle
{
    class Occupier;
    
    class GameMap
    {
        int mSize;
        int mCellsCount;
        std::vector<int> mHeightLayer;
        std::vector<Landscape> mLandscapeLayer;
        std::vector<Occupier> mBuildingLayer;
        bool mHorizontalWrapping;
        bool mVerticalWrapping;
        
    public:        
        explicit GameMap(int size);

        typedef Point Cell;

        void Height(const Cell &cell, int height);
        int Height(const Cell &cell) const;

        void LandscapeType(const Cell &cell, Landscape land);
        Landscape LandscapeType(const Cell &cell) const;

        bool HasCell(const Cell &cell) const;

        int CellToIndex(const Cell &cell) const;
        Cell IndexToCell(int index) const;

        void WrapHorizontal(bool on);
        void WrapVertical(bool on);

        Cell NullCell() const;

        int Size() const;

        struct AdjacencyIterator : public std::iterator<std::forward_iterator_tag, GameMap::Cell>
        {
            explicit constexpr AdjacencyIterator(const GameMap &map, GameMap::Cell cell, int dir)
                : mMap(map), mCell(cell), mDir(dir) {}

            const AdjacencyIterator operator++(int);
            void operator++();
            bool operator!=(const AdjacencyIterator &that) const;
            GameMap::Cell operator*() const;

        protected:
            const GameMap &mMap;
            GameMap::Cell mCell;
            int mDir;
        };

        std::pair<AdjacencyIterator, AdjacencyIterator> AdjacentCells(GameMap::Cell cell) const;

        struct CellIterator : public std::iterator<std::forward_iterator_tag, GameMap::Cell>
        {
            explicit constexpr CellIterator(const GameMap &map)
                : mMap(map) {  }

            CellIterator operator++(int);
            void operator++();
            bool operator!=(const CellIterator &that) const;
            GameMap::Cell operator*() const;
            
        protected:
            const GameMap &mMap;
        };
    };

    class Occupier
    {
    protected:
        std::vector<GameMap> mOccupiedTiles;
    public:
    };
    
    constexpr static int TileWidth = 32;
    constexpr static int TileHeight = 16;
    
    void GenerateRandomMap(GameMap &map);
}

#endif // GAMEMAP_H_
