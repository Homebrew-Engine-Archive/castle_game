#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>

#include <game/camera.h>
#include <game/point.h>

enum class Landscape;
enum class Direction;
class Surface;

namespace Castle
{
    class Terrain
    {
        
    public:
        
    };

    class Unit
    {
    public:
    };
    
    class Cell
    {
        int mHeight;
        Landscape mLandscape;
        
        
    public:
        Cell(Landscape landscape, int height);
        
        int Height() const;
        void Height(int height);
    };
    
    class GameMap
    {
        int mSize;
        std::vector<Cell> mCells;
        bool mBorderless;
        
    public:
        explicit GameMap(int size);
        
        bool Borderless() const;
        void Borderless(bool yes);

        Cell const& GetCell(int row, int col) const;
        Cell& GetCell(int row, int col);
        
        int Rows() const;
        int Cols() const;
    };

    void GenerateRandomMap(GameMap &map);
}

#endif // GAMEMAP_H_
