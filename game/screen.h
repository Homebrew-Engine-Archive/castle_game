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

        /**
         * \brief Check whether screen should be repainted.
         */
        virtual bool IsClosed() { return false; }

        /**
         * \brief React on user input.
         *
         * Only one screen at time can handle event. If it returns true, then
         * the rest screens have not any chance to handle this event too.
         *
         * It can handle even SDL_QUIT.
         *
         * \param event
         * \return True if event was completely handled or false otherwise
         */
        virtual bool HandleEvent(const SDL_Event &event) = 0;
    };

    typedef std::unique_ptr<Screen> ScreenPtr;
    
}

#endif
