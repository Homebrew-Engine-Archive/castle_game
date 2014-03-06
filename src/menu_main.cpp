#include "menu_main.h"

Button MenuMain::MakeCombatButton()
{
    auto handler = std::bind(&MenuMain::GoCombat, this);
    const CollectionData &icons = m_renderer->GetCollection("gm/icons_front_end.gm1");

    Surface released = icons.entries.at(0).surface;
    Surface pressed = icons.entries.at(16).surface;
    Surface over = icons.entries.at(5).surface;

    SDL_Rect bounds = SurfaceBounds(released);

    bounds.x = 200;
    bounds.y = 190;

    return Button(bounds, released, over, pressed, handler);
}

MenuMain::MenuMain(RootScreen *root)
    : m_root(root)
    , m_renderer(root->GetRenderer())
    , m_background(
        m_renderer->QuerySurface("gfx/frontend_main.tgx"))
{
    m_buttons.push_back(MakeCombatButton());
}

bool MenuMain::HandleEvent(const SDL_Event &event)
{
    for(Button &button : m_buttons) {
        button.HandleEvent(event);
    }
    return false;
}

void MenuMain::Draw(Surface frame)
{
    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(m_background);
    SDL_Rect bgAligned = AlignRect(bgRect, frameRect, 0, 0);

    BlitSurface(m_background, NULL, frame, &bgAligned);
    for(Button &button : m_buttons) {
        SDL_Rect buttonRect = button.GetDrawingRect(bgAligned.x, bgAligned.y);
        SurfaceROI hud(frame, &buttonRect);
        button.Draw(hud);
    }
}

void MenuMain::GoCombat()
{
    m_root->SetCurrentScreen(
        std::move(
            std::unique_ptr<Screen>(
                new MenuCombat(m_root))));
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
