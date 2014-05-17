#include "menu_main.h"

#include <game/menu_combat.h>
#include <game/filesystem.h>
#include <game/sdl_utils.h>
#include <game/screenmanager.h>
#include <game/collection.h>

namespace UI
{
    MenuMain::MenuMain(UI::ScreenManager &screenManager)
        : mScreenManager(screenManager)
        , mBackground(
            LoadTGX(
                fs::TGXFilePath("frontend_main2")))
    {
    }

    bool MenuMain::HandleEvent(const SDL_Event &event)
    {
        return false;
    }
    
    void MenuMain::Draw(Surface &frame)
    {
        Rect frameRect(frame);
        Rect bgRect(mBackground);
        Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);

        BlitSurface(mBackground, bgRect, frame, bgAligned);
    }
} // namespace UI
