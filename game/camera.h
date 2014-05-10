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
        Point mViewpoint;
        int mViewradius;
        Rect mViewport;
        Direction mDirection;
        bool mFlatView;

        void UpdateViewport();
        void UpdateViewpoint();
        void UpdateViewradius();
        
    public:
        Camera();

        Rect TileBox(const class GameMap &map, int tile) const;
        
        bool Flat() const;
        void Flat(bool yes);
        
        Direction Dir() const;
        void Dir(const Direction &dir);
        
        Point ViewPoint() const;
        void ViewPoint(const Point &point);

        void Translate(int dx, int dy);
        
        int ViewRadius() const;
        void ViewRadius(int radius);

        Rect Viewport() const;
        void Viewport(const Rect &viewport);
    };
}

#endif // CAMERA_H_
