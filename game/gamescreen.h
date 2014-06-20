#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <chrono>
#include <map>

#include <SDL.h>

#include <game/point.h>
#include <game/screen.h>
#include <game/collection.h>
#include <game/camera.h>

class Image;
enum class Landscape;

namespace castle
{
    namespace world
    {
        class Creature;
        class SimulationContext;
        class SimulationManager;
    }
    namespace ui
    {
        class ScreenManager;
    }
}

namespace castle
{
    namespace ui
    {
        class GameScreen : public Screen
        {
        public:
            explicit GameScreen(ScreenManager &screenManager);
            GameScreen(GameScreen const&) = delete;
            GameScreen& operator=(GameScreen const&) = delete;
            virtual ~GameScreen();

            void SetSimulationContext(world::SimulationContext &context);
            void Render(render::Renderer &renderer);
            bool HandleEvent(const SDL_Event &event);
            world::Camera& GetActiveCamera();

        protected:
            void RenderTile(render::Renderer &renderer, const world::Map::Cell &cell);
            void RenderCreature(render::Renderer &renderer, const world::Creature &creature);
            bool HandleKeyPress(const SDL_KeyboardEvent &event);
            bool HandleMouseButton(const SDL_MouseButtonEvent &event);
            void UpdateCamera(const render::Renderer &renderer);
            void ToggleCameraMode();
            bool IsTileSelected(const core::Point &cursor, const world::Map::Cell &cell) const;
            world::Map::Cell FindSelectedTile(const render::Renderer &renderer);
            gfx::Collection const& GetTileSet(const Landscape &landscape) const;

        private:
            ScreenManager &mScreenManager;
            gfx::Collection archer;
            gfx::Collection swordsman;
            gfx::Collection crossbowman;
            gfx::Collection buildings1;
            gfx::Collection buildings2;
            gfx::Collection workshops;
            gfx::Collection landset;
            gfx::Collection seaset;
            gfx::Collection rockset;
            gfx::Collection cliffs;
            std::chrono::steady_clock::time_point mLastCameraUpdate;
            std::map<SDL_Keycode, bool> mKeyState;
            core::Point mCursor;
            bool mCursorInvalid;
            world::Camera mCamera;
            world::SimulationContext *mSimContext;
        };
    }
}

#endif  // GAMESCREEN_H_
