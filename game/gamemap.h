#ifndef GAMEMAP_H_
#define GAMEMAP_H_

#include <vector>

enum class Direction;

class Surface;

namespace Castle
{
    class GameMap
    {
        int mRows;
        int mCols;
        std::vector<int> mTiles;
        
    public:
        GameMap(int rows, int cols);
        void Randomize(int seed);
        void Draw(Surface &surface, int viewportX, int viewportY, Direction viewportOrient, int viewportRadius);
    };
}

#endif // GAMEMAP_H_
