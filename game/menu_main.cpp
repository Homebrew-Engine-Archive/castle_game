#include "menu_main.h"

#include <game/menu_combat.h>
#include <game/filesystem.h>
#include <game/sdl_utils.h>
#include <game/screenmanager.h>

namespace UI
{
    MenuMain::MenuMain(Render::Renderer &renderer, Render::FontManager &fontManager, UI::ScreenManager &screenManager)
        : mRenderer(renderer)
        , mFontManager(fontManager)
        , mScreenManager(screenManager)
        , mBackground(
            LoadTGXSurface(
                fs::TGXFilePath("frontend_main2")))
    {
    }

    bool MenuMain::HandleEvent(const SDL_Event &event)
    {
        return false;
    }
    
    void MenuMain::Draw(Surface &frame)
    {
        SDL_Rect frameRect = SurfaceBounds(frame);
        SDL_Rect bgRect = SurfaceBounds(mBackground);
        SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);

        BlitSurface(mBackground, bgRect, frame, bgAligned);
    }
} // namespace UI
