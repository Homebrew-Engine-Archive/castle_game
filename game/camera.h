#ifndef CAMERA_H_
#define CAMERA_H_

#include <SDL.h>
#include <game/direction.h>
#include <game/rect.h>
#include <game/point.h>

namespace Castle
{
    class Camera
    {
        /** Top left corner of the screen mapped onto game map **/
        Point mViewpoint;

        /** 30x16 by default, 15x8 zoomed out **/
        Point mTileSize;

        /** North direction **/
        Direction mDirection;
        
        bool mFlatView;
        
    public:
        Camera();

        void Translate(int dx, int dy);
        
        bool Flat() const;
        void Flat(bool yes);
        
        Direction Dir() const;
        void Dir(const Direction &dir);
        
        Point ViewPoint() const;
        void ViewPoint(const Point &point);
        
        Point TileSize() const;
        void TileSize(const Point &tileSize);

        Rect Viewport(const Rect &screen) const;
    };
}

#endif // CAMERA_H_
