#ifndef SCREEN_H_
#define SCREEN_H_

#include <cstdint>
#include <SDL.h>

namespace Castle
{
    namespace Render
    {
        class Renderer;
    }

    namespace Network
    {
        class Connection;
    }
}

namespace Castle
{
    namespace UI
    {
        class Screen
        {
        public:
            virtual void Render(Render::Renderer &renderer) {}

            virtual bool CanBeStacked() const
                {return true;}
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
            virtual bool HandleEvent(const SDL_Event &event)
                {return false;}

            virtual void NewConnection(Network::Connection &connection)
                {}

            virtual void ConnectionData(Network::Connection &connection)
                {}

            virtual void ConnectionLost(Network::Connection &connection)
                {}

            virtual void EnterScreen(Screen *previous)
                {}

            virtual void LeaveScreen(Screen *next)
                {}
        };    
    }
}

#endif
