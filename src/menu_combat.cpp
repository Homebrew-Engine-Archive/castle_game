#include "menu_combat.h"

#include <memory>

#include "renderer.h"
#include "macrosota.h"
#include "filesystem.h"
#include "geometry.h"
#include "engine.h"

namespace GUI
{

    std::unique_ptr<MenuCombat> CreateMenuCombat(Castle::Engine *engine)
    {
        return make_unique<MenuCombat>(engine);
    }
    
    MenuCombat::MenuCombat(Castle::Engine *engine)
        : mEngine(engine)
        , mRenderer(engine->GetRenderer())
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

} // namespace GUI
