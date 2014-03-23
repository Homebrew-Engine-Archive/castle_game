#ifndef SCREEN_H_
#define SCREEN_H_

#include <memory>
#include <SDL.h>

class Surface;

namespace GUI
{

    struct Screen
    {
        virtual void Draw(Surface &frame) = 0;
        virtual bool HandleEvent(const SDL_Event &event) = 0;
    };

    typedef std::unique_ptr<Screen> ScreenPtr;
    
}

#endif
