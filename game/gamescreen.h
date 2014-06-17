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

namespace Castle
{
    namespace World
    {
        class Creature;
        class SimulationContext;
        class SimulationManager;
    }
    namespace UI
    {
        class ScreenManager;
    }
}

namespace Castle
{
    namespace UI
    {
        class GameScreen : public Screen
        {
            UI::ScreenManager &mScreenManager;
            Collection archer;
            Collection swordsman;
            Collection crossbowman;
            Collection buildings1;
            Collection buildings2;
            Collection workshops;
            Collection landset;
            Collection seaset;
            Collection rockset;
            Collection cliffs;
            std::chrono::steady_clock::time_point mLastCameraUpdate;
            std::map<SDL_Keycode, bool> mKeyState;
            core::Point mCursor;
            bool mCursorInvalid;
            World::Camera mCamera;
            World::SimulationContext *mSimContext;

        protected:
            void RenderTile(Render::Renderer &render, const World::Map::Cell &cell);
            void RenderCreature(Render::Renderer &renderer, const World::Creature &creature);
            bool HandleKeyPress(const SDL_KeyboardEvent &event);
            bool HandleMouseButton(const SDL_MouseButtonEvent &event);
            void UpdateCamera(const Render::Renderer &renderer);
            void ToggleCameraMode();
            World::Camera& ActiveCamera();
            bool TileSelected(const core::Point &cursor, const World::Map::Cell &cell) const;
            World::Map::Cell FindSelectedTile(const Render::Renderer &renderer);
            Collection const& GetTileSet(Landscape landscape) const;
        
        public:
            explicit GameScreen(UI::ScreenManager &screenManager);
            GameScreen(GameScreen const&) = delete;
            GameScreen& operator=(GameScreen const&) = delete;
            virtual ~GameScreen();

            void SetSimulationContext(World::SimulationContext &context);
            void Render(Render::Renderer &render);
            bool HandleEvent(const SDL_Event &event);
        };
    }
}

#endif  // GAMESCREEN_H_
