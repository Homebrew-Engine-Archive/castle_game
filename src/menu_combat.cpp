#include "menu_combat.h"

MenuCombat::MenuCombat(RootScreen *root)
    : m_root(root)
    , m_renderer(root->GetRenderer())
    , m_background(m_renderer->QuerySurface("gfx/frontend_combat.tgx"))
{
}

void MenuCombat::Draw(Surface frame)
{
    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(m_background);
    SDL_Rect bgAligned = AlignRect(bgRect, frameRect, 0, 0);

    BlitSurface(m_background, NULL, frame, &bgAligned);
}

bool MenuCombat::HandleEvent(const SDL_Event &event)
{
    UNUSED(event);
    return false;
}
