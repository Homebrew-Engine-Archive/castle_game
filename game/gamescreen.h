#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>
#include <SDL.h>
#include <game/screen.h>
#include <game/gamemap.h>
#include <game/camera.h>
#include <game/rect.h>

class CollectionData;
class Surface;

enum class CursorMode : int {
    Normal,
    Delete
};

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
    
    class GameScreen : public Screen
    {
        Render::Renderer &mRenderer;
        Render::FontManager &mFontManager;
        UI::ScreenManager &mScreenManager;
        Castle::SimulationManager &mSimulationManager;
        
        int mCursorX;
        int mCursorY;
        bool mCursorInvalid;
        bool mHiddenUI;
        CursorMode mCursorMode;
        Castle::Camera mCamera;
        int mSpriteCount;
    
    public:
        GameScreen(Render::Renderer &renderer,
                   Render::FontManager &fontManager,
                   UI::ScreenManager &screenManager,
                   Castle::SimulationManager &simulationManager);
        
        GameScreen(GameScreen const&) = delete;
        GameScreen &operator=(GameScreen const&) = delete;

        Rect TileBox(int tile) const;
        
        void DrawUnits(Surface &frame, const CollectionData &gm1);
        void DrawTerrain(Surface &frame, const CollectionData &gm1);
        void DrawTestScene(Surface &frame);
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);

        void AdjustViewport(const Rect &screen);
    
    };
    
}

#endif
