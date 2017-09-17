#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>
#include <iterator>

#include <core/point.h>
#include <core/direction.h>
#include <game/landscape.h>

namespace castle
{
    namespace world
    {
        using MapCell = core::Point;
        
        class Map
        {
            int mSize;
            int mCellsCount;
            std::vector<int> mHeightLayer;
            std::vector<Landscape> mLandscapeLayer;
            bool mHorizontalWrapping;
            bool mVerticalWrapping;
        
        private:
            int CellToIndex(const MapCell &cell) const;
            const MapCell IndexToCell(int index) const;
        
        public:
            explicit Map(int size = 0);
            Map(Map const&);
            Map& operator=(Map const&);
            Map(Map&&);
            Map& operator=(Map&&);
            virtual ~Map();

            void Height(const MapCell &cell, int height);
            int Height(const MapCell &cell) const;
            void LandscapeType(const MapCell &cell, Landscape land);
            Landscape LandscapeType(const MapCell &cell) const;
            bool HasCell(const MapCell &cell) const;
            void WrapHorizontal(bool on);
            void WrapVertical(bool on);
            const MapCell NullCell() const;
            int Size() const;
            void SetSize(int size) const;

            class AdjacencyIterator : public std::iterator<std::forward_iterator_tag, MapCell>
            {
            public:
                inline AdjacencyIterator(const Map &map, MapCell cell, int dir)
                    : mMap(map), mCell(cell), mDir(dir)
                    {}
                
                const AdjacencyIterator operator++(int);
                void operator++();
                bool operator!=(const AdjacencyIterator &that) const;
                const MapCell operator*() const;

            private:
                const Map &mMap;
                MapCell mCell;
                int mDir;
            };
        
            const std::pair<AdjacencyIterator, AdjacencyIterator> AdjacentCells(const MapCell &cell) const;

            class CellIterator : public std::iterator<std::forward_iterator_tag, MapCell>
            {
            public:
                explicit constexpr CellIterator(const Map &map, size_t cellIndex)
                    : mMap(map), mCellIndex(cellIndex)
                    {}

                const CellIterator operator++(int);
                void operator++();
                bool operator!=(const CellIterator &that) const;
                const MapCell operator*() const;
            
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
