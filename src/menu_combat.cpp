#include "menu_combat.h"
#include "macrosota.h"
#include "filesystem.h"
#include "geometry.h"

MenuCombat::MenuCombat(RootScreen *root)
    : mRoot(root)
    , mRenderer(root->GetRenderer())
{
    FilePath filepath = GetTGXFilePath("frontend_combat");
    mBackground = mRenderer->QuerySurface(filepath);
}

void MenuCombat::Draw(Surface &frame)
{
    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(mBackground);
    SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);

    BlitSurface(mBackground, NULL, frame, &bgAligned);
}

bool MenuCombat::HandleEvent(const SDL_Event &event)
{
    UNUSED(event);
    return false;
}
