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
    class Creature;
    class SimulationManager;
    class SimulationContext;
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
        Castle::Collection archer;
        Castle::Collection swordsman;
        Castle::Collection crossbowman;
        Castle::Collection buildings1;
        Castle::Collection buildings2;
        Castle::Collection workshops;
        Castle::Collection landset;
        Castle::Collection seaset;
        Castle::Collection rockset;
        Castle::Collection cliffs;
        std::chrono::steady_clock::time_point mLastCameraUpdate;
        std::map<SDL_Keycode, bool> mKeyState;
        Point mCursor;
        bool mCursorInvalid;
        Castle::Camera mCamera;
        Castle::SimulationContext *mSimContext;

    protected:
        void RenderTile(Render::Renderer &render, const Castle::GameMap::Cell &cell);
        void RenderCreature(Render::Renderer &renderer, const Castle::Creature &creature);
        bool HandleKeyPress(const SDL_KeyboardEvent &event);
        bool HandleMouseButton(const SDL_MouseButtonEvent &event);
        void UpdateCamera(const Render::Renderer &renderer);
        void ToggleCameraMode();
        Castle::Camera& ActiveCamera();
        bool TileSelected(const Point &cursor, const Castle::GameMap::Cell &cell) const;
        Castle::GameMap::Cell FindSelectedTile(const Render::Renderer &renderer);
        Castle::Collection const& GetTileSet(Landscape landscape) const;
        
    public:
        explicit GameScreen(UI::ScreenManager &screenManager);
        GameScreen(GameScreen const&) = delete;
        GameScreen& operator=(GameScreen const&) = delete;
        virtual ~GameScreen();

        void SetSimulationContext(Castle::SimulationContext &context);
        
        void Render(Render::Renderer &render);
        bool HandleEvent(const SDL_Event &event);
    };
}

#endif  // GAMESCREEN_H_
