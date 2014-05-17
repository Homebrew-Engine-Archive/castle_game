#include "camera.h"

#include <game/gm1.h>

#include <game/gamemap.h>
#include <game/rect.h>

namespace Castle
{
    Camera::Camera()
        : mPosX(0.0f)
        , mPosY(0.0f)
        , mTileSize(30, 16)
        , mDirection(Direction::North)
        , mFlatView(false)
        , mScrollLeft(0)
        , mScrollRight(0)
        , mScrollUp(0)
        , mScrollDown(0)
    {
    }
    
    void Camera::Flat(bool yes)
    {
        mFlatView = yes;
    }

    bool Camera::Flat() const
    {
        return mFlatView;
    }

    void Camera::Dir(const Direction &dir)
    {
        mDirection = dir;
    }

    Direction Camera::Dir() const
    {
        return mDirection;
    }

    void Camera::ViewPoint(const Point &viewpoint)
    {
        mPosX = viewpoint.x;
        mPosY = viewpoint.y;
    }
    
    Point Camera::ViewPoint() const
    {
        return Point(mPosX, mPosY);
    }

    Point Camera::TileSize() const
    {
        return mTileSize;
    }

    void Camera::TileSize(const Point &tileSize)
    {
        mTileSize = tileSize;
    }
    
    void Camera::MoveLeft()
    {
        mScrollLeft = 1;
    }
    
    void Camera::MoveRight()
    {
        mScrollRight = 1;
    }

    void Camera::MoveUp()
    {
        mScrollUp = 1;
    }
    
    void Camera::MoveDown()
    {
        mScrollDown = 1;
    }
    
    void Camera::Update(std::chrono::milliseconds delta)
    {
        const double ScrollSpeed = 0.8;                 // Pixel per msec
        
        mPosY += mScrollDown * ScrollSpeed * delta.count();
        mScrollDown = 0;
        
        mPosY -= mScrollUp * ScrollSpeed * delta.count();
        mScrollUp = 0;

        mPosX -= mScrollLeft * ScrollSpeed * delta.count();
        mScrollLeft = 0;

        mPosX += mScrollRight * ScrollSpeed * delta.count();
        mScrollRight = 0;
    }

    Point Camera::ScreenToWorldCoords(const Point &cursor) const
    {
        // \stub
        return Point((mPosX + cursor.x) / GM1::TileWidth,
                     (mPosY + cursor.y) / GM1::TileHeight * 2);
    }
}
