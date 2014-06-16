#include "menu_combat.h"

#include <SDL.h>

#include <game/collection.h>
#include <game/filesystem.h>
#include <game/sdl_utils.h>
#include <game/screenmanager.h>

namespace UI
{
    MenuCombat::MenuCombat(UI::ScreenManager &screenManager)
        : mScreenManager(screenManager)
        , mBackground(
            Castle::LoadTGX(
                fs::TGXFilePath("frontend_combat")))
    {
    }

    void MenuCombat::Draw(Surface &frame)
    {
        const core::Rect frameRect(frame);
        const core::Rect bgRect(mBackground);
        const core::Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);

        BlitSurface(mBackground, bgRect, frame, bgAligned);
    }

    bool MenuCombat::HandleKey(const SDL_KeyboardEvent &event)
    {
        switch(event.keysym.sym) {
        case SDLK_ESCAPE:
            mScreenManager.CloseScreen(this);
            return true;
        }
        return false;
    }
    
    bool MenuCombat::HandleEvent(const SDL_Event &event)
    {
        switch(event.type) {
        case SDL_KEYDOWN:
            return HandleKey(event.key);
        }
        return false;
    }
} // namespace UI
