#ifndef INGAMEMENU_H_
#define INGAMEMENU_H_

#include <game/screen.h>

namespace castle
{
    namespace ui
    {
        class InGameMenu : public Screen
        {
        public:
            explicit InGameMenu();
            virtual ~InGameMenu();
            virtual bool HandleEvent(const SDL_Event &event);
        };
    }
}

#endif // INGAMEMENU_H_
