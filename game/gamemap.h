#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>
#include <iterator>

#include <game/point.h>
#include <game/direction.h>
#include <game/landscape.h>

namespace Castle
{
    class GameMap
    {
        int mSize;
        int mCellsCount;
        std::vector<int> mHeightLayer;
        std::vector<Landscape> mLandscapeLayer;
        bool mHorizontalWrapping;
        bool mVerticalWrapping;

    public:
        using Cell = core::Point;
        
    private:
        int CellToIndex(const Cell &cell) const;
        const Cell IndexToCell(int index) const;
        
    public:
        explicit GameMap(int size = 0);
        GameMap(GameMap const&);
        GameMap& operator=(GameMap const&);
        GameMap(GameMap&&);
        GameMap& operator=(GameMap&&);
        virtual ~GameMap();

        void Height(const Cell &cell, int height);
        int Height(const Cell &cell) const;
        void LandscapeType(const Cell &cell, Landscape land);
        Landscape LandscapeType(const Cell &cell) const;
        bool HasCell(const Cell &cell) const;
        void WrapHorizontal(bool on);
        void WrapVertical(bool on);
        const Cell NullCell() const;
        int Size() const;
        void SetSize(int size) const;

        class AdjacencyIterator : public std::iterator<std::forward_iterator_tag, GameMap::Cell>
        {
        public:
            inline AdjacencyIterator(const GameMap &map, GameMap::Cell cell, int dir)
                : mMap(map), mCell(cell), mDir(dir) {}

            const AdjacencyIterator operator++(int);
            void operator++();
            bool operator!=(const AdjacencyIterator &that) const;
            const GameMap::Cell operator*() const;

        private:
            const GameMap &mMap;
            GameMap::Cell mCell;
            int mDir;
        };

        const std::pair<AdjacencyIterator, AdjacencyIterator> AdjacentCells(GameMap::Cell cell) const;

        class CellIterator : public std::iterator<std::forward_iterator_tag, GameMap::Cell>
        {
        public:
            explicit constexpr CellIterator(const GameMap &map, size_t cellIndex)
                : mMap(map), mCellIndex(cellIndex)
                {}

            const CellIterator operator++(int);
            void operator++();
            bool operator!=(const CellIterator &that) const;
            const GameMap::Cell operator*() const;
            
        private:
            const GameMap &mMap;
            size_t mCellIndex;
        };

        const std::pair<CellIterator, CellIterator> Cells() const;
    };

    constexpr static int TileWidth = 32;
    constexpr static int TileHeight = 16;
    
    void GenerateRandomMap(GameMap &map);
}

#endif // GAMEMAP_H_
