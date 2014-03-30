#include "menu_combat.h"

#include "SDL.h"
#include <memory>

#include "renderer.h"
#include "macrosota.h"
#include "filesystem.h"
#include "geometry.h"
#include "screenmanager.h"

namespace UI
{
    
    MenuCombat::MenuCombat(UI::ScreenManager *mgr, Render::Renderer *render)
        : mScreenMgr(mgr)
        , mRenderer(render)
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

    bool MenuCombat::IsDirty(int64_t elapsed)
    {
        return elapsed != 0;
    }

    bool MenuCombat::HandleKey(const SDL_KeyboardEvent &event)
    {
        switch(event.keysym.sym) {
        case SDLK_ESCAPE:
            mScreenMgr->CloseScreen(this);
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

    std::unique_ptr<MenuCombat> CreateMenuCombat(UI::ScreenManager *mgr, Render::Renderer *render)
    {
        return std::make_unique<MenuCombat>(mgr, render);
    }
    
} // namespace UI
