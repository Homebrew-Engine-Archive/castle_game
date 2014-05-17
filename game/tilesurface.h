#ifndef TILESURFACE_H_
#define TILESURFACE_H_

#include <game/point.h>
#include <game/surface.h>

namespace Graphics
{
    class Image
    {
        Surface mImage;
        Point mCenter;
        
    public:
        TileSurface(Surface surf, Point center)
            : mImage(std::move(surf))
            , mCenter(std::move(center))
            { }
        
        constexpr Point GetCenter() const {
            return mCenter;
        }
        
        constexpr Surface GetSurface() const {
            return mImage;
        }
        
    };

    class TileImage
    {
        Image mNormal;
        Image mFlat;

    public:
        
    };
}

#endif // TILESURFACE_H_
