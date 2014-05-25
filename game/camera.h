#ifndef CAMERA_H_
#define CAMERA_H_

#include <chrono>

#include <SDL.h>

#include <game/gamemap.h>
#include <game/direction.h>
#include <game/rect.h>
#include <game/point.h>

namespace Castle
{
    enum class CameraMode : int {
        Staggered,
        Diamond,
        Ortho
    };
    
    class Camera
    {
        double mPosX;
        double mPosY;
        
        /** 30x16 by default, 15x8 zoomed out **/
        Point mTileSize;

        int mRotation;
        
        /** North direction **/
        Direction mDirection;

        bool mFlatView;

        double mScrollX;
        double mScrollY;

        double mVerticalScrollSpeed;
        double mHorizontalScrollSpeed;

        CameraMode mCameraMode;

    public:
        Camera();

        void Update(std::chrono::milliseconds delta);
        
        void Move(int dx, int dy);
        
        void RotateLeft();
        void RotateRight();
        
        Castle::GameMap::Cell ScreenToWorldCoords(const Point &cursor) const;
        Point WorldToScreenCoords(const Castle::GameMap::Cell &cell) const;
        
        bool Flat() const;
        void Flat(bool yes);
        
        Direction Dir() const;
        void Dir(const Direction &dir);

        CameraMode Mode() const;
        void Mode(const CameraMode &mode);
        
        Point ViewPoint() const;
        void ViewPoint(const Point &point);

        Point TileSize() const;
        void TileSize(const Point &tileSize);
    };
}

#endif // CAMERA_H_
