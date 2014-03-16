#include "menu_combat.h"
#include "filesystem.h"
#include "geometry.h"

MenuCombat::MenuCombat(RootScreen *root)
    : m_root(root)
    , m_renderer(root->GetRenderer())
{
    FilePath filepath = GetTGXFilePath("frontend_combat");
    m_background = m_renderer->QuerySurface(filepath);
}

void MenuCombat::Draw(Surface frame)
{
    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(m_background);
    SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);

    BlitSurface(m_background, NULL, frame, &bgAligned);
}

bool MenuCombat::HandleEvent(const SDL_Event &event)
{
    UNUSED(event);
    return false;
}
