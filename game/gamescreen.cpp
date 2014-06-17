#include "gamescreen.h"

#include <sstream>

#include <game/color.h>
#include <game/creature.h>
#include <game/direction.h>
#include <game/filesystem.h>
#include <game/fontengine.h>
#include <game/gm1.h>
#include <game/gm1palette.h>
#include <game/landscape.h>
#include <game/make_unique.h>
#include <game/modulo.h>
#include <game/point.h>
#include <game/rect.h>
#include <game/renderer.h>
#include <game/screenmanager.h>
#include <game/simulationcontext.h>
#include <game/simulationmanager.h>
#include <game/image.h>

namespace Castle
{
    namespace UI
    {
        GameScreen::~GameScreen() = default;
        GameScreen::GameScreen(UI::ScreenManager &screenManager)
            : mScreenManager(screenManager)
            , archer(LoadGM1(fs::GM1FilePath("body_archer")))
            , swordsman(LoadGM1(fs::GM1FilePath("body_swordsman")))
            , crossbowman(LoadGM1(fs::GM1FilePath("body_crossbowman")))
            , buildings1(LoadGM1(fs::GM1FilePath("tile_buildings1")))
            , buildings2(LoadGM1(fs::GM1FilePath("tile_buildings2")))
            , workshops(LoadGM1(fs::GM1FilePath("tile_workshops")))
            , landset(LoadGM1(fs::GM1FilePath("tile_land8")))
            , seaset(LoadGM1(fs::GM1FilePath("tile_sea8")))
            , rockset(LoadGM1(fs::GM1FilePath("tile_rocks8")))
            , cliffs(LoadGM1(fs::GM1FilePath("tile_cliffs")))
            , mLastCameraUpdate(std::chrono::steady_clock::now())
            , mCursor()
            , mCursorInvalid(true)
            , mCamera()
            , mSimContext(nullptr)
        {
        }
    
        Collection const& GameScreen::GetTileSet(Landscape landscape) const
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

        void GameScreen::RenderTile(Render::Renderer &renderer, const World::Map::Cell &cell)
        {
            const World::Map &map = mSimContext->GetMap();

            const core::Size tileSize(mCamera.TileSize());
            const core::Point heightOffset(0, map.Height(cell));
            const core::Point tileWorldPos(mCamera.WorldToScreenCoords(cell));
            const core::Point tileTop(tileWorldPos - heightOffset);
            const core::Point tileBottom(tileWorldPos);
            const core::Rect tileTopRect(tileTop.x, tileTop.y, tileSize.width, tileSize.height);
            const core::Rect tileBottomRect(tileBottom.x, tileBottom.y, tileSize.width, tileSize.height);

            renderer.SetDrawColor(core::colors::Red.Opaque(100));
            renderer.DrawRhombus(tileBottomRect);
        }
    
        void GameScreen::RenderCreature(Render::Renderer &renderer, const World::Creature &creature)
        {
        
        }

        void GameScreen::Render(Render::Renderer &renderer)
        {
            if(mSimContext == nullptr) {
                return;
            }
        
            UpdateCamera(renderer);

            const World::Map &map = mSimContext->GetMap();

            const auto cells = map.Cells();
            for(auto i = cells.first; i != cells.second; ++i) {
                RenderTile(renderer, *i);
            }
        }    

        void GameScreen::ToggleCameraMode()
        {
            switch(mCamera.Mode()) {
            case World::CameraMode::Staggered:
                mCamera.Mode(World::CameraMode::Diamond);
                break;
            case World::CameraMode::Diamond:
                mCamera.Mode(World::CameraMode::Ortho);
                break;
            default:
            case World::CameraMode::Ortho:
                mCamera.Mode(World::CameraMode::Staggered);
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

            const core::Rect screenRect = renderer.GetScreenRect();
            const core::Point viewportCursor = renderer.ToViewportCoords(mCursor);
        
            if(mKeyState[SDLK_LEFT] || (!mCursorInvalid && viewportCursor.x < EdgeWidth)) {
                mCamera.Move(-1, 0);
            }
            if(mKeyState[SDLK_RIGHT] || (!mCursorInvalid && viewportCursor.x > screenRect.w - EdgeWidth)) {
                mCamera.Move(1, 0);
            }
            if(mKeyState[SDLK_UP] || (!mCursorInvalid && viewportCursor.y < EdgeWidth)) {
                mCamera.Move(0, -1);
            }
            if(mKeyState[SDLK_DOWN] || (!mCursorInvalid && viewportCursor.y > screenRect.h - EdgeWidth)) {
                mCamera.Move(0, 1);
            }
        
            using namespace std::chrono;
            mCamera.Update(duration_cast<milliseconds>(steady_clock::now() - mLastCameraUpdate));
            mLastCameraUpdate = steady_clock::now();
        }

        bool GameScreen::TileSelected(const core::Point &cursor, const World::Map::Cell &cell) const
        {
            const World::Map &map = mSimContext->GetMap();
            const Collection &tileset = GetTileSet(map.LandscapeType(cell));
        
            const size_t index = map.Height(cell);
            const GM1::EntryHeader entryHeader = tileset.GetEntryHeader(index);
            const Image &surface = tileset.GetImage(index);
            
            const core::Point tileTopLeft = mCamera.WorldToScreenCoords(cell);
            const core::Point tileTopLeftScreenSubrect = tileTopLeft - core::Point(0, map.Height(cell) - entryHeader.tileY);
            const core::Rect tileImageScreenSubrect = Translated(core::Rect(surface), tileTopLeftScreenSubrect);
            if(core::PointInRect(tileImageScreenSubrect, cursor)) {
                core::Point inside = core::ClipToRect(tileImageScreenSubrect, cursor);
                if(ExtractPixel(surface, inside) != 0) {
                    return true;
                }
            }
            return false;
        }
    
        World::Map::Cell GameScreen::FindSelectedTile(const Render::Renderer &renderer)
        {
            const core::Point viewportCursor = renderer.ToViewportCoords(mCursor);
            if(mCamera.Flat()) {
                return mCamera.ScreenToWorldCoords(viewportCursor);
            }

            const World::Map &map = mSimContext->GetMap();
            World::Map::Cell selected = map.NullCell();
            const auto cellsIters = map.Cells();
            for(auto i = cellsIters.first; i != cellsIters.second; ++i) {
                if(TileSelected(viewportCursor, *i)) {
                    selected = *i;
                }
            }
        
            return selected;
        }
    
        World::Camera& GameScreen::ActiveCamera()
        {
            return mCamera;
        }

        void GameScreen::SetSimulationContext(World::SimulationContext &context)
        {
            mSimContext = &context; 
        }
    } // namespace UI
}
