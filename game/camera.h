#ifndef CAMERA_H_
#define CAMERA_H_

#include <SDL.h>
#include <game/direction.h>

namespace Castle
{
    class Camera
    {
        SDL_Point mViewpoint;
        int mViewradius;
        SDL_Rect mViewport;
        Direction mDirection;
        bool mFlatView;

        void UpdateViewport();
        void UpdateViewpoint();
        void UpdateViewradius();
        
    public:
        Camera();

        bool Flat() const;
        void Flat(bool yes);
        
        Direction Dir() const;
        void Dir(const Direction &dir);
        
        SDL_Point ViewPoint() const;
        void ViewPoint(const SDL_Point &point);

        void Translate(int dx, int dy);
        
        int ViewRadius() const;
        void ViewRadius(int radius);

        SDL_Rect Viewport() const;
        void Viewport(const SDL_Rect &viewport);
    };
}

#endif // CAMERA_H_
