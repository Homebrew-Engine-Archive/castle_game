#include "gamescreen.h"

#include <sstream>
#include <random>

#include <game/make_unique.h>

#include <game/direction.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/landscape.h>

#include <game/renderer.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>
#include <game/surface.h>
#include <game/collection.h>
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
        , tileset(LoadGM1(fs::GM1FilePath("tile_land8")))
        , mCursor()
        , mCursorInvalid(true)
        , mCamera()
        , mSpriteCount(0)
    {
    }
    
    void GameScreen::Draw(Surface &frame)
    {
        UpdateCamera(Rect(frame));
        
        const Point selected = mCamera.ScreenToWorldCoords(mCursor);        
        const Castle::GameMap &map = mSimulationManager.GetGameMap();
        for(int i = 0; i < map.Rows(); ++i) {
            for(int j = 0; j < map.Cols(); ++j) {
                const Castle::Cell &cell = map.GetCell(i, j);
                const CollectionEntry &entry = tileset.entries[cell.Height()];
                
                Point cellCenter = mCamera.WorldToScreenCoords(Point(j, i));
                Rect tileRect(cellCenter, cellCenter + mCamera.TileSize());
                tileRect.x -= mCamera.TileSize().x / 2;
                tileRect.y -= mCamera.TileSize().y / 2;

                Rect cellRect(cellCenter, entry.surface->w, entry.surface->h);
                
                /** Offset to the center **/
                cellRect.x -= mCamera.TileSize().x / 2;
                cellRect.y -= mCamera.TileSize().y / 2;

                /** Offset to the top **/
                cellRect.y -= entry.header.tileY;

                if(Intersects(Rect(frame), cellRect)) {
                    if(!mCamera.Flat()) {
                        std::unique_ptr<SurfaceColorModSetter> setter;
                        if((selected.y == i) && (selected.x == j)) {
                            setter.reset(new SurfaceColorModSetter(entry.surface, Color(255, 128, 128)));
                        }
                        BlitSurface(entry.surface, Rect(entry.surface), frame, cellRect);
                    }
                }

                if(mCamera.Flat() && Intersects(Rect(frame), tileRect)) {
                    if(selected == Point(j, i)) {
                        DrawRhombus(frame, tileRect, Color::Yellow());
                    } else {
                        DrawRhombus(frame, tileRect, Color::Red().Opaque(100));
                    }
                }
            }
        }
    }
    
    bool GameScreen::HandleKey(const SDL_KeyboardEvent &event)
    {
        switch(event.keysym.sym) {
        case SDLK_ESCAPE:
            return false;
        case SDLK_SPACE:
            if(event.type == SDL_KEYDOWN) {
                mCamera.Flat(!mCamera.Flat());
            }
            return true;
        case SDLK_x:
            mCamera.RotateLeft();
            return true;
        case SDLK_c:
            mCamera.RotateRight();
            return true;
        default:
            return true;
        }
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
                mCursor.x = event.motion.x;
                mCursor.y = event.motion.y;
            }
            break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            mKeyState[event.key.keysym.sym] = (event.key.type == SDL_KEYDOWN);
            return HandleKey(event.key);
        }
        return false;
    }
    
    void GameScreen::UpdateCamera(const Rect &screenRect)
    {
        const int EdgeWidth = 30;
        
        if(!mCursorInvalid) {
            if(mCursor.x < EdgeWidth || mKeyState[SDLK_LEFT]) {
                mCamera.MoveLeft();
            }

            if(mCursor.x > screenRect.w - EdgeWidth || mKeyState[SDLK_RIGHT]) {
                mCamera.MoveRight();
            }

            if(mCursor.y < EdgeWidth || mKeyState[SDLK_UP]) {
                mCamera.MoveUp();
            }

            if(mCursor.y > screenRect.h - EdgeWidth || mKeyState[SDLK_DOWN]) {
                mCamera.MoveDown();
            }
        }
        
        mCamera.Update(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mLastCameraUpdate));
        mLastCameraUpdate = std::chrono::steady_clock::now();
    }

} // namespace UI
