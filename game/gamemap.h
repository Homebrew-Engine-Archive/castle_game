#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>
#include <game/point.h>
#include <iterator>

enum class Landscape;
enum class Direction;
class Surface;

namespace Castle
{
    struct MapObject
    {
    };
    
    class GameMap
    {
        int mSize;
        int mCellsCount;
        std::vector<int> mHeightLayer;
        std::vector<Landscape> mLandscapeLayer;
        std::vector<MapObject> mMapObjects;
        bool mHorizontalWrapping;
        bool mVerticalWrapping;
        
    public:        
        explicit GameMap(int size);

        typedef Point Cell;

        void Height(const Cell &cell, int height);
        int Height(const Cell &cell) const;

        void LandscapeType(const Cell &cell, Landscape land);
        Landscape LandscapeType(const Cell &cell) const;

        void PlaceObject(const MapObject &obj);
        void RemoveObject(const MapObject &obj);
        
        bool HasCell(const Cell &cell) const;

        int CellToIndex(const Cell &cell) const;
        Cell IndexToCell(int index) const;

        void WrapHorizontal(bool on);
        void WrapVertical(bool on);
        
        int Size() const;
    };

    constexpr static int TileWidth = 32;
    constexpr static int TileHeight = 16;
    
    void GenerateRandomMap(GameMap &map);

    struct EvenAdjacencyIterator : public std::iterator<std::bidirectional_iterator_tag, GameMap::Cell>
    {
        explicit constexpr EvenAdjacencyIterator(GameMap::Cell &cell) {}
    };
    
    struct OddAdjacencyIterator : public std::iterator<std::bidirectional_iterator_tag, GameMap::Cell>
    {
        explicit constexpr OddAdjacencyIterator(GameMap::Cell &cell) {}
    };
}

#endif // GAMEMAP_H_
