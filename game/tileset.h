#ifndef TILESET_H_
#define TILESET_H_

class Point;
class Surface;

namespace Graphics
{
    class TileSprite
    {
    public:
        virtual const Surface GetSurface() const = 0;
        virtual const Point GetAnchor() const = 0;
    };
    
    class TileSet
    {
    public:
        virtual const TileSprite& GetSprite() const = 0;
    };
}

#endif // TILESET_H_
