#include "ingamemenu.h"

namespace Castle
{
    namespace UI
    {
        InGameMenu::InGameMenu() = default;
        InGameMenu::~InGameMenu() = default;
    
        bool InGameMenu::HandleEvent(const SDL_Event &event)
        {
            return true;
        }
    }
}
