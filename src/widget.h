#ifndef WIDGET_H_
#define WIDGET_H_

#include <SDL.h>

class Surface;

namespace UI
{

    struct Widget
    {
        virtual bool HandleEvent(const SDL_Event &event) = 0;
        virtual void Draw(Surface &surface) = 0;
        virtual int Width() const = 0;
        virtual int Height() const = 0;
    };

}

#endif
