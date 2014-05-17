#ifndef CAMERA_H_
#define CAMERA_H_

#include <SDL.h>

#include <chrono>

#include <game/direction.h>
#include <game/rect.h>
#include <game/point.h>
#include <game/timefunction.h>

namespace Castle
{
    class Camera
    {
        double mPosX;
        double mPosY;
        
        /** 30x16 by default, 15x8 zoomed out **/
        Point mTileSize;

        /** North direction **/
        Direction mDirection;

        bool mFlatView;

        int mScrollLeft;
        int mScrollRight;
        int mScrollUp;
        int mScrollDown;
        
    public:
        Camera();

        void Update(std::chrono::milliseconds delta);
        
        void MoveLeft();
        void MoveRight();
        void MoveUp();
        void MoveDown();

        Point ScreenToWorldCoords(const Point &cursor) const;
        
        bool Flat() const;
        void Flat(bool yes);
        
        Direction Dir() const;
        void Dir(const Direction &dir);
        
        Point ViewPoint() const;
        void ViewPoint(const Point &point);
        
        Point TileSize() const;
        void TileSize(const Point &tileSize);
    };
}

#endif // CAMERA_H_
