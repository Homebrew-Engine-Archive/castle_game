#include "gamescreen.h"

#include <sstream>

#include <game/make_unique.h>

#include <game/modulo.h>
#include <game/direction.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/landscape.h>

#include <game/surface_drawing.h>
#include <game/renderer.h>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/gm1.h>
#include <game/gm1palette.h>
#include <game/textrenderer.h>
#include <game/simulationmanager.h>

namespace UI
{
    GameScreen::~GameScreen() = default;
    GameScreen::GameScreen(UI::ScreenManager &screenManager,
                           Castle::SimulationManager &simulationManager)
        : mScreenManager(screenManager)
        , mSimulationManager(simulationManager)
        , landset(LoadGM1(fs::GM1FilePath("tile_land8")))
        , seaset(LoadGM1(fs::GM1FilePath("tile_sea8")))
        , rockset(LoadGM1(fs::GM1FilePath("tile_rocks8")))
        , mCursor()
        , mCursorInvalid(true)
        , mCamera()
    {
    }
    
    CollectionData const& GameScreen::GetTileSet(Landscape landscape) const
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
    
    void GameScreen::Draw(Surface &frame)
    {
        UpdateCamera(Rect(frame));
        
        const RendererPtr renderer(SDL_CreateSoftwareRenderer(frame));
        const Castle::GameMap::Cell selected = FindSelectedTile();
        const Castle::GameMap &map = mSimulationManager.GetGameMap();

        const auto cellIters = map.Cells();
        for(auto i = cellIters.first; i != cellIters.second; ++i) {
            const Castle::GameMap::Cell cell = *i;
            const CollectionData &tileset = GetTileSet(map.LandscapeType(*i));
            const CollectionEntry &entry = tileset.entries.at(map.Height(*i));

            const Point cellCenter = mCamera.WorldToScreenCoords(*i);
            const Rect tileRect(
                cellCenter.x,
                cellCenter.y,
                mCamera.TileSize().x,
                mCamera.TileSize().y);
            Rect cellBox(cellCenter - Point(0, entry.header.tileY), entry.surface->w, entry.surface->h);

            if(!mCamera.Flat()) {
                cellBox.y -= map.Height(*i);
            }

            if(!mCamera.Flat() && Intersects(Rect(frame), cellBox)) {
                std::unique_ptr<SurfaceColorModSetter> setter;
                if(selected == *i) {
                    setter.reset(new SurfaceColorModSetter(entry.surface, Color(255, 128, 128)));
                }
                if(false /** mCamera.Scaled() **/) {
                    // \todo crop source and dest rect
                    const Rect entryRect = Translated(IntersectRects(Rect(frame), cellBox), -TopLeft(cellBox));
                    BlitSurfaceScaled(entry.surface, entryRect, frame, cellBox);
                } else {
                    BlitSurface(entry.surface, Rect(entry.surface), frame, cellBox);
                }
            }

            if(mCamera.Flat() && Intersects(Rect(frame), tileRect)) {
                const Color tileColor =
                    ((selected.x == cell.x || selected.y == cell.y)
                     ? (Colors::Yellow)
                     : (Colors::Red.Opaque(100)));
                Render::DrawRhombus(*renderer, tileRect, tileColor);
            }
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
            std::clog << "Pointed tile: " << FindSelectedTile() << std::endl;
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
    
    void GameScreen::UpdateCamera(const Rect &screenRect)
    {
        constexpr const int EdgeWidth = 10;
        if(mKeyState[SDLK_LEFT] || (!mCursorInvalid && mCursor.x < EdgeWidth)) {
            mCamera.Move(-1, 0);
        }
        if(mKeyState[SDLK_RIGHT] || (!mCursorInvalid &&  mCursor.x > screenRect.w - EdgeWidth)) {
            mCamera.Move(1, 0);
        }
        if(mKeyState[SDLK_UP] || (!mCursorInvalid && mCursor.y < EdgeWidth)) {
            mCamera.Move(0, -1);
        }
        if(mKeyState[SDLK_DOWN] || (!mCursorInvalid && mCursor.y > screenRect.h - EdgeWidth)) {
            mCamera.Move(0, 1);
        }
        
        using namespace std::chrono;
        mCamera.Update(duration_cast<milliseconds>(steady_clock::now() - mLastCameraUpdate));
        mLastCameraUpdate = steady_clock::now();
    }

    bool GameScreen::TileSelected(const Castle::GameMap &map, const Castle::GameMap::Cell &cell) const
    {
        const CollectionData &tileset = GetTileSet(map.LandscapeType(cell));
        const CollectionEntry &entry =
            tileset.entries.at(
                map.Height(cell));
            
        Point cellCenter = mCamera.WorldToScreenCoords(cell);
        Rect cellBox(cellCenter - Point(0, map.Height(cell)), entry.surface->w, entry.surface->h);
        cellBox.y -= entry.header.tileY;         // Offset to the top
        if(PointInRect(cellBox, mCursor)) {
            Point inside = ClipToRect(cellBox, mCursor);
            if(ExtractPixel(entry.surface, inside) != 0) {
                return true;
            }
        }
        return false;
    }
    
    Castle::GameMap::Cell GameScreen::FindSelectedTile()
    {
        if(mCamera.Flat()) {
            return mCamera.ScreenToWorldCoords(mCursor);
        }

        Castle::GameMap &map = mSimulationManager.GetGameMap();
        Castle::GameMap::Cell selected = map.NullCell();
        const auto cellsIters = map.Cells();
        for(auto i = cellsIters.first; i != cellsIters.second; ++i) {
            if(TileSelected(map, *i)) {
                selected = *i;
            }
        }
        
        return selected;
    }
    
    Castle::Camera& GameScreen::ActiveCamera()
    {
        return mCamera;
    }
} // namespace UI
