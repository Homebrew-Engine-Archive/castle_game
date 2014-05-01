#include "gamescreen.h"

#include <cmath>

#include <game/renderer.h>
#include <game/landscape.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/collection.h>
#include <game/direction.h>

namespace UI
{
    GameScreen::GameScreen(UI::ScreenManager *mgr, Render::Renderer *render)
        : mScreenMgr(mgr)
        , mRenderer(render)
        , mCursorX(0)
        , mCursorY(0)
        , mCursorInvalid(true)
        , mViewportX(0)
        , mViewportY(0)
        , mViewportRadius(1)
        , mViewportOrient(Direction::North)
        , mFlatView(false)
        , mLowView(false)
        , mZoomedOut(false)
        , mHiddenUI(false)
        , mCursorMode(CursorMode::Normal)
        , mClosed(false)
        , mGameMap(80, 80)
    {
    }

    GameScreen::~GameScreen()
    {
    }

    void GameScreen::DrawTestScene(Surface &frame)
    {
        SDL_Rect frameRect = SurfaceBounds(frame);
        
        fs::path bodyLord = fs::GM1FilePath("body_lord");
        const CollectionData &gm1 = mRenderer->QueryCollection(bodyLord);

        int side = sqrt(gm1.header.imageCount);
        
        int x = 0;
        int y = 0;
        
        SDL_Palette *palette = gm1.palettes.at(3).get();
        for(const CollectionEntry &entry : gm1.entries) {
            Surface face = entry.surface;
            SDL_SetSurfacePalette(face, palette);

            x += frameRect.w / side;
            if(x + face->w > frameRect.w) {
                x = 0;
                y += frameRect.h / side;
            }

            SDL_Rect whither = MakeRect(x, y, face->w, face->h);
            SDL_BlitSurface(face, NULL, frame, &whither);
        }
    }

    void GameScreen::Draw(Surface &frame)
    {
        mGameMap.Draw(frame, mViewportX, mViewportY, mViewportOrient, mViewportRadius);
    }
    
    bool GameScreen::HandleEvent(const SDL_Event &event)
    {
        switch(event.type) {
        case SDL_MOUSEMOTION:
            {
                mCursorInvalid = false;
                mCursorX = event.motion.x;
                mCursorY = event.motion.y;
            }
            break;
        case SDL_KEYDOWN:
            {
                switch(event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    return false;
                default:
                    return true;
                }
            }
            break;
        }
        return false;
    }

    void GameScreen::AdjustViewport(const SDL_Rect &screen)
    {
        if(screen.w == mCursorX)
            ++mViewportX;
        else if(0 == mCursorX)
            --mViewportX;

        if(screen.h == mCursorY)
            ++mViewportY;
        else if(0 == mCursorY)
            --mViewportY;
    }

} // namespace UI
