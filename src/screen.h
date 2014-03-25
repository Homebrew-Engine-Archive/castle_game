#ifndef SCREEN_H_
#define SCREEN_H_

#include <cstdint>
#include <memory>
#include <SDL.h>

class Surface;

namespace UI
{

    class Screen
    {
    public:
        virtual void Draw(Surface &frame) = 0;

        virtual bool IsDirty(int64_t dtime) = 0;

        virtual bool IsClosed() { return false; }

        /**
         * \brief React on user input.
         * \param event
         * \return True if handled or false otherwise
         */
        virtual bool HandleEvent(const SDL_Event &event) = 0;
    };

    typedef std::unique_ptr<Screen> ScreenPtr;
    
}

#endif
