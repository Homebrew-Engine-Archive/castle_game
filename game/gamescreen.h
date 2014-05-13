#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>
#include <chrono>

#include <SDL.h>

#include <game/screen.h>
#include <game/gamemap.h>
#include <game/camera.h>
#include <game/rect.h>

class CollectionData;
class Surface;

namespace Castle
{
    class Engine;
}

namespace Render
{
    class Renderer;
    class FontManager;
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
    class SmoothValue
    {
        std::chrono::milliseconds mLatency;
        std::chrono::milliseconds mElapsed;
        double mFrom;
        double mTo;
    public:
        SmoothValue(std::chrono::milliseconds latency, int from, int to);
        void Update(std::chrono::milliseconds elapsed);
        int Get() const;
    };

    class GameScreen : public Screen
    {
        Render::Renderer &mRenderer;
        Render::FontManager &mFontManager;
        UI::ScreenManager &mScreenManager;
        Castle::SimulationManager &mSimulationManager;

        std::unique_ptr<SmoothValue> mCameraXVelocity;
        std::unique_ptr<SmoothValue> mCameraYVelocity;

        std::chrono::steady_clock::time_point mLastCameraUpdate;
        
        Point mCursor;
        bool mCursorInvalid;
        Castle::Camera mCamera;
        int mSpriteCount;
    
    public:
        GameScreen(Render::Renderer &renderer,
                   Render::FontManager &fontManager,
                   UI::ScreenManager &screenManager,
                   Castle::SimulationManager &simulationManager);
        
        GameScreen(GameScreen const&) = delete;
        GameScreen &operator=(GameScreen const&) = delete;
        ~GameScreen();
        
        void DrawUnits(Surface &frame, const CollectionData &gm1);
        void DrawTerrain(Surface &frame, const CollectionData &gm1);
        void DrawTestScene(Surface &frame);
        void DrawCameraInfo(Surface &frame);
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);

        void UpdateCamera(std::chrono::milliseconds time, const Rect &screen);
        void UpdateCameraVelocity(std::unique_ptr<SmoothValue> &cameraVelocity,
                                  std::chrono::milliseconds dtime,
                                  int cursor, int bounds);
    };
}

#endif
