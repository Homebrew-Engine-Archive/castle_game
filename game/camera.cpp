#include "camera.h"

#include <game/gamemap.h>
#include <game/rect.h>

namespace Castle
{
    Camera::Camera()
        : mViewpoint(0, 0)
        , mTileSize(30, 16)
        , mDirection(Direction::North)
        , mFlatView(false)
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
        mViewpoint = viewpoint;
    }

    void Camera::Translate(int dx, int dy)
    {
        mViewpoint += Point(dx, dy);
    }
    
    Point Camera::ViewPoint() const
    {
        return mViewpoint;
    }

    Point Camera::TileSize() const
    {
        return mTileSize;
    }

    void Camera::TileSize(const Point &tileSize)
    {
        mTileSize = tileSize;
    }

    Rect Camera::Viewport(const Rect &screen) const
    {
        return Rect();
    }
}
