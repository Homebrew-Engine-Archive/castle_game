#include "ingamemenu.h"

namespace castle
{
    namespace ui
    {
        InGameMenu::InGameMenu() = default;
        InGameMenu::~InGameMenu() = default;
    
        bool InGameMenu::HandleEvent(const SDL_Event &event)
        {
            return true;
        }
    }
}