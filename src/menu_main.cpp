#include "menu_main.h"
#include "menu_combat.h"
#include "macrosota.h"
#include "filesystem.h"
#include "collection.h"
#include "geometry.h"
#include "screenmanager.h"

namespace UI
{

    std::unique_ptr<MenuMain> CreateMenuMain(UI::ScreenManager *mgr, Render::Renderer *render)
    {
        return make_unique<MenuMain>(mgr, render);
    }

    UI::Button MenuMain::MakeCombatButton()
    {
        FilePath filepath = GetGM1FilePath("icons_front_end");
        const CollectionData &icons = mRenderer->QueryCollection(filepath);
    
        Surface released = icons.entries.at(0).surface;
        Surface pressed = icons.entries.at(16).surface;
        Surface over = icons.entries.at(5).surface;
    
        SDL_Rect bounds = SurfaceBounds(released);
        bounds.x = 200;
        bounds.y = 190;

        auto handler = std::bind(&MenuMain::GoCombat, this);
    
        return UI::Button(bounds, released, over, pressed, handler);
    }

    MenuMain::MenuMain(UI::ScreenManager *mgr, Render::Renderer *render)
        : mScreenMgr(mgr)
        , mRenderer(render)
    {
        FilePath filepath = GetTGXFilePath("frontend_main");
        mBackground = mRenderer->QuerySurface(filepath);
        mButtons.push_back(MakeCombatButton());
    }

    bool MenuMain::HandleEvent(const SDL_Event &event)
    {
        for(UI::Button &button : mButtons) {
            button.HandleEvent(event);
        }
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
        for(UI::Button &button : mButtons) {
            SDL_Rect buttonRect = button.GetDrawingRect(bgAligned.x, bgAligned.y);
            SurfaceROI hud(frame, &buttonRect);
            button.Draw(hud);
        }
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
    
} // namespace UI
