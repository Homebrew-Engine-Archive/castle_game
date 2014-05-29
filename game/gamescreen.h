#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <chrono>
#include <map>

#include <SDL.h>

#include <game/point.h>
#include <game/screen.h>
#include <game/collection.h>
#include <game/camera.h>

class Rect;
class Surface;
enum class Landscape;

namespace Castle
{
    class SimulationManager;
}

namespace UI
{
    class ScreenManager;
}

namespace UI
{
    class GameScreen : public Screen
    {
        UI::ScreenManager &mScreenManager;
        Castle::SimulationManager &mSimulationManager;
        CollectionData landset;
        CollectionData seaset;
        CollectionData rockset;
        std::chrono::steady_clock::time_point mLastCameraUpdate;
        std::map<SDL_Keycode, bool> mKeyState;
        Point mCursor;
        bool mCursorInvalid;
        Castle::Camera mCamera;
        CollectionData const& GetTileSet(Landscape landscape) const;
        
    public:
        GameScreen(UI::ScreenManager &screenManager,
                   Castle::SimulationManager &simulationManager);
        
        GameScreen(GameScreen const&) = delete;
        GameScreen& operator=(GameScreen const&) = delete;
        virtual ~GameScreen();
        
        bool TileSelected(const Castle::GameMap &map, const Castle::GameMap::Cell &cell) const;
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
        bool HandleKeyPress(const SDL_KeyboardEvent &event);
        bool HandleMouseButton(const SDL_MouseButtonEvent &event);
        void UpdateCamera(const Rect &screenRect);
        void ToggleCameraMode();
        Castle::Camera& ActiveCamera();
        Castle::GameMap::Cell FindSelectedTile();
    };
}

#endif  // GAMESCREEN_H_
