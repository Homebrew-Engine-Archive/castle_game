#include "camera.h"

#include <game/gm1.h>
#include <game/modulo.h>

#include <game/gamemap.h>
#include <game/rect.h>

namespace Castle
{
    Camera::Camera()
        : mPosX(0.0f)
        , mPosY(0.0f)
        , mTileSize(Point(GM1::TileWidth, GM1::TileHeight) * 2)
        , mDirection(Direction::North)
        , mFlatView(true)
        , mScrollX(0)
        , mScrollY(0)
        , mVerticalScrollSpeed(1)
        , mHorizontalScrollSpeed(2)
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
        mScrollX -= mHorizontalScrollSpeed;
    }
    
    void Camera::MoveRight()
    {
        mScrollX += mHorizontalScrollSpeed;
    }

    void Camera::MoveUp()
    {
        mScrollY -= mVerticalScrollSpeed;
    }
    
    void Camera::MoveDown()
    {
        mScrollY += mVerticalScrollSpeed;
    }
    
    void Camera::RotateLeft()
    {
        mDirection = RotatedLeft(mDirection);
    }

    void Camera::RotateRight()
    {
        mDirection = RotatedRight(mDirection);
    }
    
    void Camera::Update(std::chrono::milliseconds delta)
    {
        mPosX += mScrollX * delta.count();
        mScrollX = 0.0f;
        
        mPosY += mScrollY * delta.count();
        mScrollY = 0.0f;
    }
    
    Point Camera::ScreenToWorldCoords(const Point &cursor) const
    {
        // \stub
        int y = (mPosY + cursor.y) / mTileSize.y * 2;
        int x = (mPosX + cursor.x + (core::Even(y) ? (mTileSize.x / 2) : 0)) / mTileSize.x;
        return Point(x, y);
    }

    Point Camera::WorldToScreenCoords(const Point &cell) const
    {
        // \stub
        return -ViewPoint() +
            Point(cell.x * mTileSize.x + core::Modulo(cell.y, 2) * (mTileSize.x / 2),
                  cell.y * mTileSize.y / 2);
    }
}
