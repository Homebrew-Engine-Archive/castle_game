#include "camera.h"

#include <iostream>

#include <game/gm1.h>

#include <core/modulo.h>
#include <core/size.h>
#include <core/point.h>

namespace castle
{
    namespace world
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

        void Camera::ViewPoint(const core::Point &viewpoint)
        {
            mPosX = viewpoint.X();
            mPosY = viewpoint.Y();
        }
    
        const core::Point Camera::ViewPoint() const
        {
            return core::Point(mPosX, mPosY);
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
    
        const world::Map::Cell Camera::ScreenToworldCoords(const core::Point &cursor) const
        {
            switch(mCameraMode) {
            case CameraMode::Staggered:
                {
                    /** stretched and unprojected cursor position **/
                    const double px = mPosX + cursor.X() - mTileSize.Width() / 2;
                    const double py = (mPosY + cursor.Y()) * mTileSize.Height() / mTileSize.Height() - mTileSize.Height();

                    /** rotated counterclockwise by 45 degrees **/
                    const double rx = px + py;
                    const double ry = py - px;

                    /** coordinates in square-cell grid **/
                    const int x = round(rx / mTileSize.Width());
                    const int y = round(ry / mTileSize.Width());

                    /** rotated clockwise by 45 degrees **/
                    const int tx = floor((x - y) * mTileSize.Height() / static_cast<double>(mTileSize.Width()));
                    const int ty = x + y;
        
                    return world::Map::Cell(tx, ty);
                }
            
            case CameraMode::Diamond:
                {
                    // \todo bring here original matrix and implement affine transformations
                    const int h = mTileSize.Height() / 2;
                    const int w = mTileSize.Width() / 2;
                    const int q = mTileSize.Width();
                    const int r = 0;
                    const int x = cursor.X() + mPosX;
                    const int y = cursor.Y() + mPosY;
                    const int z = 2 * w * h;
                    return world::Map::Cell((-h*q - r*w + h*x + w*y) / z,
                                            (h*q - r*w - h*x + w*y) / z);
                }

            case CameraMode::Ortho:
            default:
                {
                    return world::Map::Cell((ViewPoint().X() + cursor.X()) / mTileSize.Width(),
                                            (ViewPoint().Y() + cursor.Y()) / mTileSize.Height());
                }
            }
        }
    
        const core::Point Camera::worldToScreenCoords(const world::Map::Cell &cell) const
        {
            switch(mCameraMode) {
            case CameraMode::Staggered:
                {
                    const int w = mTileSize.Width() / 2;
                    const int h = mTileSize.Height() / 2;
                    return -ViewPoint() +
                        core::Point(2*w*cell.X() + w*core::Mod(cell.Y(), 2),
                                    h*cell.Y());
                }
            
            case CameraMode::Diamond:
                {
                    const int w = mTileSize.Width() / 2;
                    const int h = mTileSize.Height() / 2;
                    const int q = mTileSize.Width() / 2;
                    const int r = 0;
                    return -ViewPoint() +
                        core::Point(w*cell.X() - w*cell.Y() + q,
                                    h*cell.X() + h*cell.Y() + r);
                }

            default:
            case CameraMode::Ortho:
                return -ViewPoint() +
                    core::Point(mTileSize.Width() * cell.X(),
                                mTileSize.Height() * cell.Y());
            }
        }
    }
}
