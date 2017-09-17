#include "gamescreen.h"

#include <sstream>

#include <core/color.h>
#include <core/direction.h>
#include <core/modulo.h>
#include <core/point.h>
#include <core/rect.h>
#include <core/image.h>
#include <core/palette.h>

#include <gm1/gm1.h>

#include <game/creature.h>
#include <game/fontengine.h>
#include <game/landscape.h>
#include <game/renderengine.h>
#include <game/renderer.h>
#include <game/screenmanager.h>
#include <game/simulationmanager.h>
#include <game/vfs.h>

namespace castle
{
    namespace ui
    {
        GameScreen::~GameScreen() = default;
        GameScreen::GameScreen(ScreenManager &screenManager)
            : mScreenManager(screenManager)
            , archer(gfx::LoadGM1(vfs::GetGM1Filename("body_archer")))
            , landset(gfx::LoadGM1(vfs::GetGM1Filename("tile_land8")))
            , seaset(gfx::LoadGM1(vfs::GetGM1Filename("tile_sea8")))
            , rockset(gfx::LoadGM1(vfs::GetGM1Filename("tile_rocks8")))
            , cliffs(gfx::LoadGM1(vfs::GetGM1Filename("tile_cliffs")))
            , mLastCameraUpdate(std::chrono::steady_clock::now())
            , mCursor()
            , mCursorInvalid(true)
            , mCamera()
            , mSimManager(nullptr)
        {
        }
    
        gfx::Collection const& GameScreen::GetTileSet(const Landscape &landscape) const
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

        void GameScreen::RenderTile(render::Renderer &renderer, const world::MapCell &cell)
        {
            const world::Map &map = mSimManager->GetMap();
            
            const core::Size tileSize(mCamera.TileSize());
            const core::Point heightOffset(0, map.Height(cell));
            const core::Point tileworldPos(mCamera.worldToScreenCoords(cell));
            const core::Point tileTop(tileworldPos - heightOffset);
            const core::Point tileBottom(tileworldPos);
            const core::Rect tileTopRect(tileTop.X(), tileTop.Y(), tileSize.Width(), tileSize.Height());
            const core::Rect tileBottomRect(tileBottom.X(), tileBottom.Y(), tileSize.Width(), tileSize.Height());

            const Landscape landscape = map.LandscapeType(cell);
            const gfx::Collection &collection = GetTileSet(landscape);
            const size_t height = map.Height(cell);
            const core::Image img = collection.GetImage(height);
            renderer.BindImage(img);
            renderer.Blit(core::Rect(img.Width(), img.Height()), tileBottom);
        }
    
        void GameScreen::RenderCreature(render::Renderer &renderer, const world::Creature &creature)
        {
            const world::CreatureClass &cc = creature.GetClass();
            const world::CreatureState &state = creature.GetState();

            
        }

        void GameScreen::Render(render::Renderer &renderer)
        {
            if(mSimManager == nullptr) {
                return;
            }
        
            UpdateCamera(renderer);

            const world::Map &map = mSimManager->GetMap();

            const auto cells = map.Cells();
            for(auto i = cells.first; i != cells.second; ++i) {
                RenderTile(renderer, *i);
            }
        }    

        void GameScreen::ToggleCameraMode()
        {
            switch(mCamera.Mode()) {
            case world::CameraMode::Staggered:
                mCamera.Mode(world::CameraMode::Diamond);
                break;
            case world::CameraMode::Diamond:
                mCamera.Mode(world::CameraMode::Ortho);
                break;
            default:
            case world::CameraMode::Ortho:
                mCamera.Mode(world::CameraMode::Staggered);
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
            mCursor.SetX(event.x);
            mCursor.SetY(event.y);
            mCursorInvalid = false;
            if((event.button == SDL_BUTTON_LEFT) && (event.state == SDL_PRESSED)) {
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
                    mCursor.SetX(event.motion.x);
                    mCursor.SetY(event.motion.y);
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
    
        void GameScreen::UpdateCamera(const render::Renderer &renderer)
        {
            constexpr const int EdgeWidth = 10;

            const core::Rect screenRect = renderer.GetScreenRect();
            const core::Point viewportCursor = renderer.ToViewportCoords(mCursor);
        
            if(mKeyState[SDLK_LEFT] || (!mCursorInvalid && viewportCursor.X() < EdgeWidth)) {
                mCamera.Move(-1, 0);
            }
            if(mKeyState[SDLK_RIGHT] || (!mCursorInvalid && viewportCursor.X() > screenRect.Width() - EdgeWidth)) {
                mCamera.Move(1, 0);
            }
            if(mKeyState[SDLK_UP] || (!mCursorInvalid && viewportCursor.Y() < EdgeWidth)) {
                mCamera.Move(0, -1);
            }
            if(mKeyState[SDLK_DOWN] || (!mCursorInvalid && viewportCursor.Y() > screenRect.Height() - EdgeWidth)) {
                mCamera.Move(0, 1);
            }
        
            using namespace std::chrono;
            mCamera.Update(duration_cast<milliseconds>(steady_clock::now() - mLastCameraUpdate));
            mLastCameraUpdate = steady_clock::now();
        }

        bool GameScreen::IsTileSelected(const core::Point &cursor, const world::MapCell &cell) const
        {
            const world::Map &map = mSimManager->GetMap();
            const gfx::Collection &tileset = GetTileSet(map.LandscapeType(cell));
        
            const size_t index = map.Height(cell);
            const gm1::EntryHeader entryHeader = tileset.GetEntryHeader(index);
            const core::Image &image = tileset.GetImage(index);

            const core::Point tileTopLeft = mCamera.worldToScreenCoords(cell);
            const core::Point tileTopLeftScreenSubrect = tileTopLeft - core::Point(0, map.Height(cell) - entryHeader.tileY);
            const core::Rect imageRect(image.Width(), image.Height());
            const core::Rect tileImageScreenSubrect = core::Translated(
                imageRect,
                tileTopLeftScreenSubrect.X(),
                tileTopLeftScreenSubrect.Y());
            if(core::PointInRect(tileImageScreenSubrect, cursor)) {
                core::Point inside = core::ClipToRect(tileImageScreenSubrect, cursor);
                if(ExtractPixel(image, inside) != 0) {
                    return true;
                }
            }
            return false;
        }
    
        world::MapCell GameScreen::FindSelectedTile(const render::Renderer &renderer)
        {
            const core::Point viewportCursor = renderer.ToViewportCoords(mCursor);
            if(mCamera.Flat()) {
                return mCamera.ScreenToworldCoords(viewportCursor);
            }

            const world::Map &map = mSimManager->GetMap();
            world::MapCell selected = map.NullCell();
            const auto cellsIters = map.Cells();
            for(auto i = cellsIters.first; i != cellsIters.second; ++i) {
                if(IsTileSelected(viewportCursor, *i)) {
                    selected = *i;
                }
            }
        
            return selected;
        }
    
        world::Camera& GameScreen::GetActiveCamera()
        {
            return mCamera;
        }

        void GameScreen::SetSimulation(world::SimulationManager &simsim)
        {
            mSimManager = &simsim;
        }
    } // namespace ui
}
