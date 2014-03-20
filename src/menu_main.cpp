#include "menu_main.h"
#include "menu_combat.h"
#include "macrosota.h"
#include "filesystem.h"
#include "collection.h"

Button MenuMain::MakeCombatButton()
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
    
    return Button(bounds, released, over, pressed, handler);
}

MenuMain::MenuMain(RootScreen *root)
    : mRoot(root)
    , mRenderer(root->GetRenderer())
{
    FilePath filepath = GetTGXFilePath("frontend_main");
    mBackground = m_renderer->QuerySurface(filepath);
    mButtons.push_back(MakeCombatButton());
}

bool MenuMain::HandleEvent(const SDL_Event &event)
{
    for(Button &button : mButtons) {
        button.HandleEvent(event);
    }
    return false;
}

void MenuMain::Draw(Surface &frame)
{
    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(mBackground);
    SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);

    BlitSurface(mBackground, NULL, frame, &bgAligned);
    for(Button &button : mButtons) {
        SDL_Rect buttonRect = button.GetDrawingRect(bgAligned.x, bgAligned.y);
        SurfaceROI hud(frame, &buttonRect);
        button.Draw(hud);
    }
}

void MenuMain::GoCombat()
{
    mRoot->SetCurrentScreen(
        std::unique_ptr<Screen>(
            new MenuCombat(mRoot)));
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
