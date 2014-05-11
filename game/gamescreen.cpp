#include "gamescreen.h"

#include <sstream>
#include <random>

#include <game/color.h>
#include <game/rect.h>
#include <game/renderer.h>
#include <game/landscape.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/collection.h>
#include <game/direction.h>

#include <game/gm1.h>
#include <game/gm1palette.h>

#include <game/fontmanager.h>
#include <game/textrenderer.h>

namespace UI
{
    GameScreen::GameScreen(Render::Renderer &renderer,
                           Render::FontManager &fontManager,
                           UI::ScreenManager &screenManager,
                           Castle::SimulationManager &simulationManager)
        : mRenderer(renderer)
        , mFontManager(fontManager)
        , mScreenManager(screenManager)
        , mSimulationManager(simulationManager)
        , mCursorX(0)
        , mCursorY(0)
        , mCursorInvalid(true)
        , mHiddenUI(false)
        , mCursorMode(CursorMode::Normal)
        , mCamera()
        , mSpriteCount(0)
    {
    }

    void GameScreen::DrawUnits(Surface &frame, const CollectionData &gm1)
    {
        std::default_random_engine g(0);
        std::uniform_int_distribution<int> paletteIndex(1, GM1::CollectionPaletteCount - 1);
        std::uniform_int_distribution<int> entryIndex(0, gm1.entries.size() - 1);

        const Rect frameRect = SurfaceBounds(frame);

        int x = 0;
        int y = 0;

        bool evenRow = true;

        for(;;) {
            SDL_Palette *palette = gm1.palettes.at(paletteIndex(g)).get();
            const CollectionEntry &entry = gm1.entries.at(entryIndex(g));
        
            Surface face = entry.surface;
            SDL_SetSurfacePalette(face, palette);
            
            x += GM1::TileWidth;
            if(x > frameRect.w + face->w) {
                evenRow = !evenRow;
                y += GM1::TileHeight / 2;
                x = (evenRow ? (GM1::TileWidth / 2) : 0);
            }

            if(y > frameRect.h) {
                break;
            }
            
            Rect whither(x, y, face->w, face->h);
            BlitSurface(face, SurfaceBounds(face), frame, whither);
            mSpriteCount++;
        }
    }

    void GameScreen::DrawTerrain(Surface &frame, const CollectionData &gm1)
    {
        int x = 0;
        int y = 0;

        bool evenRow = true;
        
        const Rect frameRect(frame->w, frame->h);

        std::default_random_engine g(0);
        std::uniform_int_distribution<int> entryIndex(0, gm1.entries.size() - 1);

        for(;;) {
            const CollectionEntry &entry = gm1.entries.at(entryIndex(g));
            Surface face = entry.surface;
            
            x += GM1::TileWidth;
            if(x > frameRect.w + face->w) {
                evenRow = !evenRow;
                y += GM1::TileHeight / 2;
                x = (evenRow ? (GM1::TileWidth / 2) : 0);
            }

            if(y > frameRect.h) {
                break;
            }

            Rect whither(x - gm1.header.anchorX, y - gm1.header.anchorY - entry.header.tileY, face->w, face->h);
            BlitSurface(face, SurfaceBounds(face), frame, whither);
            mSpriteCount++;
        }
    }
    
    void GameScreen::DrawTestScene(Surface &frame)
    {
        mSpriteCount = 0;
        
        static fs::path tileLand = fs::GM1FilePath("tile_land8");
        static const CollectionData &tileLandGM1 = mRenderer.QueryCollection(tileLand);
        DrawTerrain(frame, tileLandGM1);
        
        static fs::path bodyLord = fs::GM1FilePath("body_archer");
        static const CollectionData &bodyLordGM1 = mRenderer.QueryCollection(bodyLord);
        DrawUnits(frame, bodyLordGM1);

        Render::TextRenderer textRenderer(frame);
        textRenderer.Translate(0, frame->h - 20);
        textRenderer.SetColor(Color::Red());
        textRenderer.SetFont(mFontManager.DefaultFont());

        std::ostringstream oss;
        oss << "Scene objects: " << mSpriteCount;
        std::string text = oss.str();
        FillFrame(frame, textRenderer.CalculateTextRect(text), Color::Black().Opaque(200));
        textRenderer.PutString(text);
    }

    void GameScreen::Draw(Surface &frame)
    {
        DrawTestScene(frame);
    }
    
    bool GameScreen::HandleEvent(const SDL_Event &event)
    {
        switch(event.type) {
        case SDL_WINDOWEVENT:
            {
                if(event.window.event == SDL_WINDOWEVENT_LEAVE) {
                    mCursorInvalid = true;
                }
            }
            break;
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

    void GameScreen::AdjustViewport(const Rect &screen)
    {
        const int speed = 3;
        if(!mCursorInvalid) {
            if(mCursorX < 10) {
                mCamera.Translate(-speed, 0);
            }
            if(mCursorX > screen.w - 10) {
                mCamera.Translate(speed, 0);
            }
            if(mCursorY < 10) {
                mCamera.Translate(0, -speed);
            }
            if(mCursorY > screen.h - 10) {
                mCamera.Translate(0, speed);
            }
        }
    }

} // namespace UI
