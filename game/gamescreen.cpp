#include "gamescreen.h"

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
        , tileset(LoadGM1(fs::GM1FilePath("tile_land8")))
        , mCursor()
        , mCursorInvalid(true)
        , mCamera()
        , mSpriteCount(0)
        , mZoomed(false)
    {
    }
    
    void GameScreen::Draw(Surface &frame)
    {
        UpdateCamera(Rect(frame));
        
        const Point selected = mCamera.ScreenToWorldCoords(mCursor);        
        const Castle::GameMap &map = mSimulationManager.GetGameMap();
        for(int i = 0; i < map.Size(); ++i) {
            for(int j = 0; j < map.Size(); ++j) {
                const Castle::Cell &cell = map.GetCell(i, j);
                const CollectionEntry &entry = tileset.entries[cell.Height()];

                Point cellCenter = mCamera.WorldToScreenCoords(Point(j, i));

                Rect tileRect(cellCenter, cellCenter + mCamera.TileSize());

                Rect cellRect(cellCenter, entry.surface->w, entry.surface->h);

                /** Offset to the top **/
                cellRect.y -= entry.header.tileY;

                if(!mCamera.Flat() && Intersects(Rect(frame), cellRect)) {
                    std::unique_ptr<SurfaceColorModSetter> setter;
                    if((selected.y == i) && (selected.x == j)) {
                        setter.reset(new SurfaceColorModSetter(entry.surface, Color(255, 128, 128)));
                    }
                    BlitSurface(entry.surface, Rect(entry.surface), frame, cellRect);
                }

                if(mCamera.Flat() && Intersects(Rect(frame), tileRect)) {
                    Color tileColor = Color::Red().Opaque(100);
                    if(selected == Point(j, i)) {
                        tileColor = Color::Yellow();
                    }
                    DrawRhombus(frame, tileRect, tileColor);
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
        case SDLK_z:
            if(event.type == SDL_KEYDOWN) {
                mZoomed = !mZoomed;
                if(mZoomed) {
                    mCamera.TileSize(Point(15, 8));
                } else {
                    mCamera.TileSize(Point(30, 16));
                }
            }
            return true;
        default:
            return false;
        }
    }

    bool GameScreen::HandleMouseButton(const SDL_MouseButtonEvent &event)
    {
        mCursor.x = event.x;
        mCursor.y = event.y;
        mCursorInvalid = false;
        
        if(event.button == SDL_BUTTON_LEFT && event.state == SDL_PRESSED) {
            std::clog << "Tile: " << mCamera.ScreenToWorldCoords(mCursor) << std::endl;
            return true;
        }

        return false;
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
        case SDL_MOUSEBUTTONDOWN:
            return HandleMouseButton(event.button);
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
        static const int EdgeWidth = 30;
                
        if(!mCursorInvalid) {
            if(mKeyState[SDLK_LEFT] || mCursor.x < EdgeWidth) {
                mCamera.Move(-1, 0);
            }

            if(mKeyState[SDLK_RIGHT] || mCursor.x > screenRect.w - EdgeWidth) {
                mCamera.Move(1, 0);
            }

            if(mKeyState[SDLK_UP] || mCursor.y < EdgeWidth) {
                mCamera.Move(0, -1);
            }

            if(mKeyState[SDLK_DOWN] || mCursor.y > screenRect.h - EdgeWidth) {
                mCamera.Move(0, 1);
            }
        }
        
        mCamera.Update(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mLastCameraUpdate));
        mLastCameraUpdate = std::chrono::steady_clock::now();
    }

    Castle::Camera& GameScreen::ActiveCamera()
    {
        return mCamera;
    }
} // namespace UI
