#include "camera.h"

#include <iostream>
#include <game/gm1.h>
#include <game/modulo.h>
#include <game/point.h>

namespace Castle
{
    Camera::Camera()
        : mPosX(0.0f)
        , mPosY(0.0f)
        , mTileSize(Point(32, 16))
        , mRotation(0)
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
    
    void Camera::Move(int dx, int dy)
    {
        mScrollX += mHorizontalScrollSpeed * dx;
        mScrollY += mVerticalScrollSpeed * dy;
    }
    
    void Camera::RotateLeft()
    {
        mRotation -= 1;
    }

    void Camera::RotateRight()
    {
        mRotation += 1;
    }
    
    void Camera::Update(std::chrono::milliseconds delta)
    {
        mPosX += mScrollX * delta.count();
        mScrollX = 0.0f;
        
        mPosY += mScrollY * delta.count();
        mScrollY = 0.0f;
    }
    
    GameMap::Cell Camera::ScreenToWorldCoords(const Point &cursor) const
    {
        /** stretched and unprojected cursor position **/
        const double px = mPosX + cursor.x - mTileSize.x / 2;
        const double py = (mPosY + cursor.y) * mTileSize.x / mTileSize.y - mTileSize.y;

        /** rotated counterclockwise by 45 degrees **/
        const double rx = px + py;
        const double ry = py - px;

        /** coordinates in square-cell grid **/
        const int x = round(rx / mTileSize.x);
        const int y = round(ry / mTileSize.x);

        /** rotated clockwise by 45 degrees **/
        const int tx = floor((x - y) * mTileSize.y / double(mTileSize.x));
        const int ty = x + y;
        
        return GameMap::Cell(tx, ty);
    }
    
    Point Camera::WorldToScreenCoords(const GameMap::Cell &cell) const
    {
        return -ViewPoint() +
            Point(cell.x * mTileSize.x + core::Mod(cell.y, 2) * (mTileSize.x / 2),
                  cell.y * mTileSize.y / 2);
}
}
