#ifndef LANDSCAPE_H_
#define LANDSCAPE_H_

#include <SDL2/SDL.h>

enum class ViewMode : Uint32 {
    Normal,
    Flat,
    Low
};

enum class ViewDirection : Uint32 {
    Front,
    Left,
    Back,
    Right
};

enum class LandscapeClass : Uint32 {
    Dirt,
    
};

struct AbstractTile
{
    virtual bool IsPassable() const = 0;
    virtual void Draw(const SDL_Rect *rect, ViewMode mode = Normal, ViewDirection dir = Front) = 0;
};

#endif
