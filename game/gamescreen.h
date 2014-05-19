#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>
#include <chrono>
#include <map>

#include <SDL.h>

#include <game/collection.h>

#include <game/screen.h>
#include <game/gamemap.h>
#include <game/camera.h>
#include <game/rect.h>
#include <game/point.h>

class CollectionData;
class Surface;

namespace Castle
{
    class Engine;
}

namespace Castle
{
    class GameMap;
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
        CollectionData tileset;

        std::chrono::steady_clock::time_point mLastCameraUpdate;
        
        std::map<SDL_Keycode, bool> mKeyState;

        Point mCursor;
        bool mCursorInvalid;
        Castle::Camera mCamera;
        int mSpriteCount;
    
    public:
        GameScreen(UI::ScreenManager &screenManager,
                   Castle::SimulationManager &simulationManager);
        
        GameScreen(GameScreen const&) = delete;
        GameScreen &operator=(GameScreen const&) = delete;
        ~GameScreen();
        
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
        bool HandleKey(const SDL_KeyboardEvent &event);

        void UpdateCamera(const Rect &screenRect);
    };
}

#endif
