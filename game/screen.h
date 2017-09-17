#ifndef SCREEN_H_
#define SCREEN_H_

#include <cstdint>
#include <SDL.h>

namespace castle
{
    namespace render
    {
        class Renderer;
    }

    namespace net
    {
        class Connection;
    }
}

namespace castle
{
    namespace ui
    {
        class Screen
        {
        public:
            virtual void Render(render::Renderer &renderer) {}

            virtual bool CanBeStacked() const
                {return true;}
            /**
             * \brief React on user input.
             *
             * Only one screen at time can handle event. If it returns true, then
             * the rest screens have not any chance to handle this event too.
             *
             * It can handle even SDL_QuiT.
             *
             * \param event
             * \return True if event was completely handled or false otherwise
             */
            virtual bool HandleEvent(const SDL_Event &event)
                {return false;}

            virtual void NewConnection(net::Connection &connection)
                {}

            virtual void ConnectionData(net::Connection &connection)
                {}

            virtual void ConnectionLost(net::Connection &connection)
                {}

            virtual void EnterScreen(Screen *previous)
                {}

            virtual void LeaveScreen(Screen *next)
                {}
        };    
    }
}

#endif
