#ifndef WIDGET_H_
#define WIDGET_H_

#include "surface.h"

class Widget
{
public:
    virtual bool MouseMotionEvent(const SDL_MouseMotionEvent &event) = 0;
    virtual bool MouseButtonEvent(const SDL_MouseButtonEvent &event) = 0;
    virtual void Draw(Surface surface) = 0;
    virtual int Width() const = 0;
    virtual int Height() const = 0;
};

#endif
