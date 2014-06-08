#include "gamescreen.h"

#include <sstream>

#include <game/make_unique.h>

#include <game/modulo.h>
#include <game/direction.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/landscape.h>

#include <game/fontmanager.h>
#include <game/surface_drawing.h>
#include <game/renderer.h>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/gm1.h>
#include <game/gm1palette.h>
#include <game/textrenderer.h>
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

    void GameScreen::Render(Render::Renderer &renderer)
    {
        UpdateCamera(renderer);

        std::ostringstream oss;
        oss << mCamera.ViewPoint();
        mScreenManager.Console().LogMessage(oss.str());
        
        const Castle::GameMap::Cell selected = FindSelectedTile(renderer);
        const Castle::GameMap &map = Castle::SimulationManager::Instance().GetGameMap();

        const auto cellIters = map.Cells();
        for(auto i = cellIters.first; i != cellIters.second; ++i) {
            const Castle::GameMap::Cell cell = *i;
            const Castle::Collection &tileset = GetTileSet(map.LandscapeType(*i));

            const size_t index = map.Height(cell);
            const GM1::EntryHeader entryHeader = tileset.GetEntryHeader(index);
            const Surface &surface = tileset.GetSurface(index);

            const Point tileTopLeft = mCamera.WorldToScreenCoords(*i);
            const Rect tileRect = FromPointAndSize(tileTopLeft, mCamera.TileSize());

            const Point heightOffset = (mCamera.Flat()
                                        ? (Point(0, 0))
                                        : (Point(0, map.Height(*i))));

            const Point tileTopLeftScreenSubrect = tileTopLeft - Point(0, entryHeader.tileY) - heightOffset;
            const Rect tileSurfaceScreenSubrect = Translated(Rect(surface), tileTopLeftScreenSubrect);
            
            if(!mCamera.Flat()) {
                const Surface &cliff = ((map.LandscapeType(*i) == Landscape::River)
                                        ? (cliffs.GetSurface(34))
                                        : (cliffs.GetSurface(index)));

                const Rect cliffSubRect(
                    0,
                    SurfaceHeight(cliff) - map.Height(*i),
                    SurfaceWidth(cliff),
                    map.Height(*i) + mCamera.TileSize().y / 2);

                const Rect tileCliffSubRect(
                    tileSurfaceScreenSubrect.x,
                    tileSurfaceScreenSubrect.y + mCamera.TileSize().y / 2,
                    cliffSubRect.w,
                    cliffSubRect.h);

                renderer.BindSurface(cliff);
                renderer.Blit(cliffSubRect, tileCliffSubRect);
                
                renderer.BindSurface(surface);
                renderer.Blit(Rect(surface), tileSurfaceScreenSubrect);

                if(selected == *i) {
                    renderer.FillRhombus(tileSurfaceScreenSubrect, Colors::Red.Opaque(100));
                }
            }

            if(mCamera.Flat()) {
                const Color tileColor =
                    ((selected.x == cell.x || selected.y == cell.y)
                     ? (Colors::Yellow)
                     : (Colors::Red.Opaque(100)));
                renderer.DrawRhombus(tileRect, tileColor);
            }

            continue;
            const Surface &sprite = archer.GetSurface(index);
            const GM1::Palette &palette = archer.GetPalette(Castle::PaletteName::Blue);
            const Rect spriteBox = Translated(Rect(sprite), tileTopLeft - archer.Anchor() - heightOffset + mCamera.TileSize() / 2);
            renderer.Clip(spriteBox);
            renderer.BindPalette(palette);
            renderer.BindSurface(sprite);
            renderer.Blit(Rect(sprite), Rect(sprite));
            renderer.Unclip();
            //break;
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
        const Castle::GameMap &map = Castle::SimulationManager::Instance().GetGameMap();
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

        Castle::GameMap &map = Castle::SimulationManager::Instance().GetGameMap();
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
} // namespace UI
