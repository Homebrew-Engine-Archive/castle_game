#include "gamescreen.h"

#include <sstream>
#include <random>

#include <game/make_unique.h>

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
#include <game/simulationmanager.h>

namespace UI
{
    SmoothValue::SmoothValue(std::chrono::milliseconds latency, int from, int to)
        : mLatency(latency)
        , mElapsed(0)
        , mFrom(from)
        , mTo(to)
    {}
    
    void SmoothValue::Update(std::chrono::milliseconds elapsed)
    {
        mElapsed += elapsed;
    }

    int SmoothValue::Get() const
    {
        return mFrom + (mTo - mFrom) * std::min(mElapsed, mLatency).count() / mLatency.count();
    }
}

namespace UI
{
    GameScreen::~GameScreen() = default;
    GameScreen::GameScreen(Render::Renderer &renderer,
                           Render::FontManager &fontManager,
                           UI::ScreenManager &screenManager,
                           Castle::SimulationManager &simulationManager)
        : mRenderer(renderer)
        , mFontManager(fontManager)
        , mScreenManager(screenManager)
        , mSimulationManager(simulationManager)
        , mCursor(0, 0)
        , mCursorInvalid(true)
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
            SDL_Palette *palette = gm1.palettes[paletteIndex(g)].get();
            const CollectionEntry &entry = gm1.entries[entryIndex(g)];
        
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

    void GameScreen::DrawCameraInfo(Surface &frame)
    {
        DrawFrame(frame, Rect(mCamera.ViewPoint()), Color::Red());
    }
    
    void GameScreen::Draw(Surface &frame)
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::milliseconds sinceLastCameraUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastCameraUpdate);

        mLastCameraUpdate = now;
        
        UpdateCamera(sinceLastCameraUpdate, Rect(frame->w, frame->h));

        DrawCameraInfo(frame);
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
                mCursor = Point(event.motion.x, event.motion.y);
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

    void GameScreen::UpdateCameraVelocity(std::unique_ptr<SmoothValue> &cameraVelocity,
                                          std::chrono::milliseconds dtime,
                                          int cursor, int bounds)
    {
        const std::chrono::milliseconds Inertia = std::chrono::seconds(1);

        const int EdgeGap = 30;
        
        const int MinVelocity = 1;
        const int MaxVelocity = 5;
        
        bool affected = false;

        int velocity = 0;

        if(cameraVelocity) {
            cameraVelocity->Update(dtime);
        }
        
        if(cursor < EdgeGap) {
            affected = !affected;
            velocity = MaxVelocity;
        }

        if(cursor > bounds - EdgeGap) {
            affected = !affected;
            velocity = -MaxVelocity;
        }

        if(!affected) {
            cameraVelocity = nullptr;
        } else if(!cameraVelocity) {
            cameraVelocity = std::make_unique<SmoothValue>(Inertia, MinVelocity, -velocity);
        }
    }
    
    void GameScreen::UpdateCamera(std::chrono::milliseconds dtime, const Rect &screen)
    {
        if(!mCursorInvalid) {
            UpdateCameraVelocity(mCameraXVelocity, dtime, mCursor.x, screen.w);
            UpdateCameraVelocity(mCameraYVelocity, dtime, mCursor.y, screen.h);
            
            int deltaX = (mCameraXVelocity ? mCameraXVelocity->Get() : 0);
            int deltaY = (mCameraYVelocity ? mCameraYVelocity->Get() : 0);

            mCamera.Translate(deltaX, deltaY);
        } else {
            mCameraXVelocity = nullptr;
            mCameraYVelocity = nullptr;
        }
    }
} // namespace UI
