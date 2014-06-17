#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>
#include <iterator>

#include <game/point.h>
#include <game/direction.h>
#include <game/landscape.h>

namespace Castle
{
    namespace World
    {
        class Map
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
            explicit Map(int size = 0);
            Map(Map const&);
            Map& operator=(Map const&);
            Map(Map&&);
            Map& operator=(Map&&);
            virtual ~Map();

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

            class AdjacencyIterator : public std::iterator<std::forward_iterator_tag, Map::Cell>
            {
            public:
                inline AdjacencyIterator(const Map &map, Map::Cell cell, int dir)
                    : mMap(map), mCell(cell), mDir(dir)
                    {}
                
                const AdjacencyIterator operator++(int);
                void operator++();
                bool operator!=(const AdjacencyIterator &that) const;
                const Map::Cell operator*() const;

            private:
                const Map &mMap;
                Map::Cell mCell;
                int mDir;
            };
        
            const std::pair<AdjacencyIterator, AdjacencyIterator> AdjacentCells(Map::Cell cell) const;

            class CellIterator : public std::iterator<std::forward_iterator_tag, Map::Cell>
            {
            public:
                explicit constexpr CellIterator(const Map &map, size_t cellIndex)
                    : mMap(map), mCellIndex(cellIndex)
                    {}

                const CellIterator operator++(int);
                void operator++();
                bool operator!=(const CellIterator &that) const;
                const Map::Cell operator*() const;
            
            private:
                const Map &mMap;
                size_t mCellIndex;
            };

            const std::pair<CellIterator, CellIterator> Cells() const;
        };

        constexpr static int TileWidth = 32;
        constexpr static int TileHeight = 16;
    
        void GenerateTestMap(Map &map);
    }
}

#endif // GAMEMAP_H_
