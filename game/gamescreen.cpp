#include "gamescreen.h"

#include <sstream>

#include <game/make_unique.h>

#include <game/direction.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/landscape.h>

#include <game/renderer.h>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/gm1.h>
#include <game/gm1palette.h>
#include <game/textrenderer.h>
#include <game/simulationmanager.h>
#include <game/fontmanager.h>

namespace UI
{
    GameScreen::~GameScreen() = default;
    GameScreen::GameScreen(UI::ScreenManager &screenManager,
                           Castle::SimulationManager &simulationManager)
        : mScreenManager(screenManager)
        , mSimulationManager(simulationManager)
        , tileset(LoadGM1(fs::GM1FilePath("tile_data")))
        , mCursor()
        , mCursorInvalid(true)
        , mCamera()
    {
    }
    
    void GameScreen::Draw(Surface &frame)
    {
        UpdateCamera(Rect(frame));

        const RendererPtr renderer(SDL_CreateSoftwareRenderer(frame));
        const Castle::GameMap::Cell selected = FindSelectedTile();
        const Castle::GameMap &map = mSimulationManager.GetGameMap();
        for(int y = 0; y < map.Size(); ++y) {
            for(int x = 0; x < map.Size(); ++x) {
                const Castle::GameMap::Cell cell(x, y);
                const CollectionEntry &entry =
                    tileset.entries.at(
                        GetLandscapeIndex(
                            map.LandscapeType(cell)));

                Point cellCenter = mCamera.WorldToScreenCoords(cell);

                Rect tileRect(cellCenter, cellCenter + mCamera.TileSize());

                Rect cellBox(cellCenter, entry.surface->w, entry.surface->h);

                cellBox.y -= entry.header.tileY;

                if(!mCamera.Flat() && Intersects(Rect(frame), cellBox)) {
                    std::unique_ptr<SurfaceColorModSetter> setter;
                    if(selected == cell) {
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
                    Color tileColor = Colors::Red.Opaque(100);
                    if((selected.x == cell.x) || (selected.y == cell.y)) {
                        tileColor = Colors::Yellow;
                    }
                    DrawRhombus(renderer.get(), tileRect, tileColor);
                }
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
        static const int EdgeWidth = 30;
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
        mCamera.Update(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mLastCameraUpdate));
        mLastCameraUpdate = std::chrono::steady_clock::now();
    }

    Castle::GameMap::Cell GameScreen::FindSelectedTile()
    {
        if(mCamera.Flat()) {
            return mCamera.ScreenToWorldCoords(mCursor);
        }
        
        const Castle::GameMap &map = mSimulationManager.GetGameMap();
        for(int y = map.Size() - 1; y >= 0; --y) {
            for(int x = map.Size() - 1; x >= 0; --x) {
                const Castle::GameMap::Cell cell(x, y);
                const CollectionEntry &entry =
                    tileset.entries.at(
                        GetLandscapeIndex(
                            map.LandscapeType(cell)));
                Point cellCenter = mCamera.WorldToScreenCoords(cell);
                Rect cellBox(cellCenter, entry.surface->w, entry.surface->h);
                cellBox.y -= entry.header.tileY;         // Offset to the top
                if(PointInRect(cellBox, mCursor)) {
                    Point inside = ClipToRect(cellBox, mCursor);
                    if(ExtractPixel(entry.surface, inside) != 0) {
                        return cell;
                    }
                }
            }
        }
        
        return map.NullCell();
    }
    
    Castle::Camera& GameScreen::ActiveCamera()
    {
        return mCamera;
    }
} // namespace UI
