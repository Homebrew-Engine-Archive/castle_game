#include "gamescreen.h"

#include <sstream>

#include <game/make_unique.h>

#include <game/modulo.h>
#include <game/direction.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/landscape.h>

#include <game/simulationcontext.h>
#include <game/creature.h>
#include <game/fontmanager.h>
#include <game/renderer.h>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/gm1.h>
#include <game/gm1palette.h>
#include <game/simulationmanager.h>
#include <game/screenmanager.h>

namespace UI
{
    GameScreen::~GameScreen() = default;
    GameScreen::GameScreen(UI::ScreenManager &screenManager)
        : mScreenManager(screenManager)
        , archer(Castle::LoadGM1(fs::GM1FilePath("body_archer")))
        , swordsman(Castle::LoadGM1(fs::GM1FilePath("body_swordsman")))
        , crossbowman(Castle::LoadGM1(fs::GM1FilePath("body_crossbowman")))
        , buildings1(Castle::LoadGM1(fs::GM1FilePath("tile_buildings1")))
        , buildings2(Castle::LoadGM1(fs::GM1FilePath("tile_buildings2")))
        , workshops(Castle::LoadGM1(fs::GM1FilePath("tile_workshops")))
        , landset(Castle::LoadGM1(fs::GM1FilePath("tile_land8")))
        , seaset(Castle::LoadGM1(fs::GM1FilePath("tile_sea8")))
        , rockset(Castle::LoadGM1(fs::GM1FilePath("tile_rocks8")))
        , cliffs(Castle::LoadGM1(fs::GM1FilePath("tile_cliffs")))
        , mLastCameraUpdate(std::chrono::steady_clock::now())
        , mCursor()
        , mCursorInvalid(true)
        , mCamera()
        , mSimContext(nullptr)
    {
    }
    
    Castle::Collection const& GameScreen::GetTileSet(Landscape landscape) const
    {
        switch(landscape) {
        case Landscape::Sea:
        case Landscape::River:
        case Landscape::Mash:
        case Landscape::Ford:
        case Landscape::Ripple:
        case Landscape::Swamp:
        case Landscape::Oil:
            return seaset;
        case Landscape::Pebbles:
        case Landscape::Rocks:
        case Landscape::Stones:
        case Landscape::Boulders:
            return rockset;
        default:
            return landset;
        }
    }

    void GameScreen::RenderTile(Render::Renderer &renderer, const Castle::GameMap::Cell &cell)
    {
        const Castle::GameMap &map = mSimContext->GetGameMap();

        const core::Size tileSize(mCamera.TileSize());
        const Point heightOffset(0, map.Height(cell));
        const Point tileWorldPos(mCamera.WorldToScreenCoords(cell));
        const Point tileTop(tileWorldPos - heightOffset);
        const Point tileBottom(tileWorldPos);
        const Rect tileTopRect(tileTop.x, tileTop.y, tileSize.width, tileSize.height);
        const Rect tileBottomRect(tileBottom.x, tileBottom.y, tileSize.width, tileSize.height);

        renderer.DrawRhombus(tileBottomRect, Colors::Red.Opaque(100));
    }
    
    void GameScreen::RenderCreature(Render::Renderer &renderer, const Castle::Creature &creature)
    {
        
    }

    void GameScreen::Render(Render::Renderer &renderer)
    {
        if(mSimContext == nullptr) {
            return;
        }
        
        UpdateCamera(renderer);

        const Castle::GameMap &map = mSimContext->GetGameMap();

        const auto cells = map.Cells();
        for(auto i = cells.first; i != cells.second; ++i) {
            RenderTile(renderer, *i);
        }
    }    

    void GameScreen::ToggleCameraMode()
    {
        switch(mCamera.Mode()) {
        case Castle::CameraMode::Staggered:
            mCamera.Mode(Castle::CameraMode::Diamond);
            break;
        case Castle::CameraMode::Diamond:
            mCamera.Mode(Castle::CameraMode::Ortho);
            break;
        default:
        case Castle::CameraMode::Ortho:
            mCamera.Mode(Castle::CameraMode::Staggered);
            break;
        }
    }
    
    bool GameScreen::HandleKeyPress(const SDL_KeyboardEvent &event)
    {
        switch(event.keysym.sym) {
        case SDLK_SPACE:
            mCamera.Flat(!mCamera.Flat());
            return true;
            
        case SDLK_x:
            mCamera.RotateLeft();
            return true;
            
        case SDLK_c:
            mCamera.RotateRight();
            return true;
            
        case SDLK_z:
            {
                ToggleCameraMode();
                return true;
            }
        }

        return false;
    }

    bool GameScreen::HandleMouseButton(const SDL_MouseButtonEvent &event)
    {
        mCursor.x = event.x;
        mCursor.y = event.y;
        mCursorInvalid = false;
        if(event.button == SDL_BUTTON_LEFT && event.state == SDL_PRESSED) {
            return true;
        }
        return false;
    }
    
    bool GameScreen::HandleEvent(const SDL_Event &event)
    {
        switch(event.type) {
        case SDL_WINDOWEVENT:
            {
                switch(event.window.event) {
                case SDL_WINDOWEVENT_LEAVE:
                    mCursorInvalid = true;
                    return true;
                default:
                    return false;
                }
            }
        case SDL_MOUSEBUTTONDOWN:
            {
                return HandleMouseButton(event.button);
            }
        case SDL_MOUSEMOTION:
            {
                mCursorInvalid = false;
                mCursor.x = event.motion.x;
                mCursor.y = event.motion.y;
                return true;
            }
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            {
                mKeyState[event.key.keysym.sym] = (event.key.type == SDL_KEYDOWN);
                if(event.key.type == SDL_KEYDOWN) {
                    return HandleKeyPress(event.key);
                }
                return false;
            }
        }
        return false;
    }
    
    void GameScreen::UpdateCamera(const Render::Renderer &renderer)
    {
        constexpr const int EdgeWidth = 10;

        const Rect screenRect = renderer.GetScreenRect();
        const Point projectedCursor = renderer.ClipPoint(mCursor);
        
        if(mKeyState[SDLK_LEFT] || (!mCursorInvalid && projectedCursor.x < EdgeWidth)) {
            mCamera.Move(-1, 0);
        }
        if(mKeyState[SDLK_RIGHT] || (!mCursorInvalid && projectedCursor.x > screenRect.w - EdgeWidth)) {
            mCamera.Move(1, 0);
        }
        if(mKeyState[SDLK_UP] || (!mCursorInvalid && projectedCursor.y < EdgeWidth)) {
            mCamera.Move(0, -1);
        }
        if(mKeyState[SDLK_DOWN] || (!mCursorInvalid && projectedCursor.y > screenRect.h - EdgeWidth)) {
            mCamera.Move(0, 1);
        }
        
        using namespace std::chrono;
        mCamera.Update(duration_cast<milliseconds>(steady_clock::now() - mLastCameraUpdate));
        mLastCameraUpdate = steady_clock::now();
    }

    bool GameScreen::TileSelected(const Point &cursor, const Castle::GameMap::Cell &cell) const
    {
        const Castle::GameMap &map = mSimContext->GetGameMap();
        const Castle::Collection &tileset = GetTileSet(map.LandscapeType(cell));
        
        const size_t index = map.Height(cell);
        const GM1::EntryHeader entryHeader = tileset.GetEntryHeader(index);
        const Surface &surface = tileset.GetSurface(index);
            
        const Point tileTopLeft = mCamera.WorldToScreenCoords(cell);
        const Point tileTopLeftScreenSubrect = tileTopLeft - Point(0, map.Height(cell) - entryHeader.tileY);
        const Rect tileSurfaceScreenSubrect = Translated(Rect(surface), tileTopLeftScreenSubrect);
        if(PointInRect(tileSurfaceScreenSubrect, cursor)) {
            Point inside = ClipToRect(tileSurfaceScreenSubrect, cursor);
            if(ExtractPixel(surface, inside) != 0) {
                return true;
            }
        }
        return false;
    }
    
    Castle::GameMap::Cell GameScreen::FindSelectedTile(const Render::Renderer &renderer)
    {
        const Point projectedCursor = renderer.ClipPoint(mCursor);
        if(mCamera.Flat()) {
            return mCamera.ScreenToWorldCoords(projectedCursor);
        }

        const Castle::GameMap &map = mSimContext->GetGameMap();
        Castle::GameMap::Cell selected = map.NullCell();
        const auto cellsIters = map.Cells();
        for(auto i = cellsIters.first; i != cellsIters.second; ++i) {
            if(TileSelected(projectedCursor, *i)) {
                selected = *i;
            }
        }
        
        return selected;
    }
    
    Castle::Camera& GameScreen::ActiveCamera()
    {
        return mCamera;
    }

    void GameScreen::SetSimulationContext(Castle::SimulationContext &context)
    {
        mSimContext = &context; 
    }
} // namespace UI
