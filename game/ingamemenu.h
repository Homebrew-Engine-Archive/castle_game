#ifndef INGAMEMENU_H_
#define INGAMEMENU_H_

#include <game/screen.h>

namespace UI
{
    class InGameMenu : public Screen
    {
    public:
        InGameMenu();
        virtual ~InGameMenu();
        virtual bool HandleEvent(const SDL_Event &event);
    };
}

#endif // INGAMEMENU_H_
