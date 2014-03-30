#include "menu_main.h"
#include "menu_combat.h"
#include "make_unique.h"
#include "filesystem.h"
#include "collection.h"
#include "geometry.h"
#include "screenmanager.h"

namespace UI
{

    MenuMain::MenuMain(UI::ScreenManager *mgr, Render::Renderer *render)
        : mScreenMgr(mgr)
        , mRenderer(render)
    {
        FilePath filepath = GetTGXFilePath("frontend_main");
        mBackground = mRenderer->QuerySurface(filepath);
    }

    bool MenuMain::HandleEvent(const SDL_Event &event)
    {
        return false;
    }

    bool MenuMain::IsDirty(int64_t elapsed)
    {
        return elapsed != 0;
    }
    
    void MenuMain::Draw(Surface &frame)
    {
        SDL_Rect frameRect = SurfaceBounds(frame);
        SDL_Rect bgRect = SurfaceBounds(mBackground);
        SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);

        BlitSurface(mBackground, NULL, frame, &bgAligned);
    }

    void MenuMain::GoCombat()
    {
        mScreenMgr->PushScreen(UI::CreateMenuCombat(mScreenMgr, mRenderer));
    }

    void MenuMain::GoEconomics()
    {
    }
    
    void MenuMain::GoBuilder()
    {
    }

    void MenuMain::GoLoadGame()
    {
    }

    void MenuMain::GoExit()
    {
    }


    std::unique_ptr<MenuMain> CreateMenuMain(UI::ScreenManager *mgr, Render::Renderer *render)
    {
        return std::make_unique<MenuMain>(mgr, render);
    }

    
} // namespace UI