#include "camera.h"

#include <iostream>
#include <game/gm1.h>
#include <game/modulo.h>
#include <game/size.h>
#include <game/point.h>

namespace Castle
{
    Camera::Camera()
        : mPosX(0.0f)
        , mPosY(0.0f)
        , mTileSize(core::Size(32, 16))
        , mRotation(0)
        , mDirection(core::Direction::North)
        , mStartDirection(core::Direction::North)
        , mFlatView(false)
        , mScrollX(0)
        , mScrollY(0)
        , mVerticalScrollSpeed(1.5)
        , mHorizontalScrollSpeed(2.2)
        , mCameraMode(CameraMode::Staggered)
    {
    }

    CameraMode Camera::Mode() const
    {
        return mCameraMode;
    }
    
    void Camera::Mode(const CameraMode &mode)
    {
        mCameraMode = mode;
    }
    
    void Camera::Flat(bool yes)
    {
        mFlatView = yes;
    }

    bool Camera::Flat() const
    {
        return mFlatView;
    }

    double Camera::AngleRotation() const
    {
        return DirectionToRadians(mDirection) - DirectionToRadians(mStartDirection);
    }

    void Camera::Dir(const core::Direction &dir)
    {
        mDirection = dir;
    }

    core::Direction Camera::Dir() const
    {
        return mDirection;
    }

    void Camera::ViewPoint(const Point &viewpoint)
    {
        mPosX = viewpoint.x;
        mPosY = viewpoint.y;
    }
    
    const Point Camera::ViewPoint() const
    {
        return Point(mPosX, mPosY);
    }

    const core::Size Camera::TileSize() const
    {
        return mTileSize;
    }

    void Camera::TileSize(const core::Size &tileSize)
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
    
    const GameMap::Cell Camera::ScreenToWorldCoords(const Point &cursor) const
    {
        switch(mCameraMode) {
        case CameraMode::Staggered:
            {
                /** stretched and unprojected cursor position **/
                const double px = mPosX + cursor.x - mTileSize.width / 2;
                const double py = (mPosY + cursor.y) * mTileSize.height / mTileSize.height - mTileSize.height;

                /** rotated counterclockwise by 45 degrees **/
                const double rx = px + py;
                const double ry = py - px;

                /** coordinates in square-cell grid **/
                const int x = round(rx / mTileSize.width);
                const int y = round(ry / mTileSize.width);

                /** rotated clockwise by 45 degrees **/
                const int tx = floor((x - y) * mTileSize.height / static_cast<double>(mTileSize.width));
                const int ty = x + y;
        
                return GameMap::Cell(tx, ty);
            }
            
        case CameraMode::Diamond:
            {
                // \todo bring here original matrix and implement affine transformations
                const int h = mTileSize.height / 2;
                const int w = mTileSize.width / 2;
                const int q = mTileSize.width;
                const int r = 0;
                const int x = cursor.x + mPosX;
                const int y = cursor.y + mPosY;
                const int z = 2 * w * h;
                return GameMap::Cell((-h*q - r*w + h*x + w*y) / z,
                                     (h*q - r*w - h*x + w*y) / z);
            }

        case CameraMode::Ortho:
        default:
            {
                return GameMap::Cell((ViewPoint().x + cursor.x) / mTileSize.width,
                                     (ViewPoint().y + cursor.y) / mTileSize.height);
            }
        }
    }
    
    const Point Camera::WorldToScreenCoords(const GameMap::Cell &cell) const
    {
        switch(mCameraMode) {
        case CameraMode::Staggered:
            {
                const int w = mTileSize.width / 2;
                const int h = mTileSize.height / 2;
                return -ViewPoint() +
                    Point(2*w*cell.x + w*core::Mod(cell.y, 2),
                          h*cell.y);
            }
            
        case CameraMode::Diamond:
            {
                const int w = mTileSize.width / 2;
                const int h = mTileSize.height / 2;
                const int q = mTileSize.width / 2;
                const int r = 0;
                return -ViewPoint() +
                    Point(w*cell.x - w*cell.y + q,
                          h*cell.x + h*cell.y + r);
            }

        default:
        case CameraMode::Ortho:
            return -ViewPoint() +
                Point(mTileSize.width * cell.x,
                      mTileSize.height * cell.y);
        }
    }
}
