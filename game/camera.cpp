#include "camera.h"

#include <game/sdl_utils.h>

namespace Castle
{
    Camera::Camera()
        : mViewpoint {0, 0}
        , mViewradius(12)
        , mViewport {0, 0, 0, 0}
        , mDirection(Direction::North)
        , mFlatView(false)
    {
        UpdateViewport();
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

    void Camera::ViewPoint(const SDL_Point &viewpoint)
    {
        mViewpoint = viewpoint;
        UpdateViewport();
    }

    void Camera::Translate(int dx, int dy)
    {
        mViewpoint = ShiftPoint(mViewpoint, dx, dy);
        UpdateViewport();
    }
    
    SDL_Point Camera::ViewPoint() const
    {
        return mViewpoint;
    }

    int Camera::ViewRadius() const
    {
        return mViewradius;
    }

    void Camera::ViewRadius(int radius)
    {
        mViewradius = radius;
        UpdateViewport();
    }

    SDL_Rect Camera::Viewport() const
    {
        return mViewport;
    }

    void Camera::Viewport(const SDL_Rect &viewport)
    {
        mViewport = viewport;
        UpdateViewpoint();
        UpdateViewradius();
    }

    void Camera::UpdateViewport()
    {
        mViewport = MakeRect(ShiftPoint(mViewpoint, -mViewradius, -mViewradius),
                             ShiftPoint(mViewpoint, mViewradius, mViewradius));
    }

    void Camera::UpdateViewpoint()
    {
        mViewpoint = RectCenter(mViewport);
    }

    void Camera::UpdateViewradius()
    {
        mViewradius = std::min(mViewport.w / 2, mViewport.h / 2);
    }
}
