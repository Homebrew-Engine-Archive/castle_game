#ifndef CAMERA_H_
#define CAMERA_H_

#include <chrono>

#include <SDL.h>

#include <game/size.h>
#include <game/gamemap.h>
#include <game/direction.h>
#include <game/rect.h>

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
        core::Size mTileSize;

        int mRotation;
        
        /** North direction **/
        core::Direction mDirection;
        const core::Direction mStartDirection;

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
        
        const Castle::GameMap::Cell ScreenToWorldCoords(const Point &cursor) const;
        const Point WorldToScreenCoords(const Castle::GameMap::Cell &cell) const;
        
        bool Flat() const;
        void Flat(bool yes);

        double AngleRotation() const;
        
        core::Direction Dir() const;
        void Dir(const core::Direction &dir);

        CameraMode Mode() const;
        void Mode(const CameraMode &mode);
        
        const Point ViewPoint() const;
        void ViewPoint(const Point &point);

        const core::Size TileSize() const;
        void TileSize(const core::Size &tileSize);
    };
}

#endif // CAMERA_H_
