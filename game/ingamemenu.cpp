#include "ingamemenu.h"

#include <game/surface.h>

namespace UI
{
    InGameMenu::InGameMenu() = default;
    InGameMenu::~InGameMenu() = default;
    
    void InGameMenu::Draw(Surface &frame)
    {
    }

    bool InGameMenu::HandleEvent(const SDL_Event &event)
    {
        return true;
    }
}
