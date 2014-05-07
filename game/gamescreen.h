#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>
#include <SDL.h>
#include <game/screen.h>
#include <game/gamemap.h>

class CollectionData;

enum class Direction;
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
}

namespace UI
{

    class ScreenManager;
    
    class GameScreen : public Screen
    {
        UI::ScreenManager &mScreenMgr;
        Render::FontManager &mFontMgr;
        Render::Renderer &mRenderer;
        int mCursorX;
        int mCursorY;
        bool mCursorInvalid;
        int mViewportX;
        int mViewportY;
        int mViewportRadius;
        Direction mViewportOrient;
        bool mFlatView;
        bool mLowView;
        bool mZoomedOut;
        bool mHiddenUI;
        CursorMode mCursorMode;
        bool mClosed;
        Castle::GameMap mGameMap;
        int mSpriteCount;
    
    public:
        GameScreen(UI::ScreenManager &screenMgr, Render::FontManager &fontMgr, Render::Renderer &render);
        GameScreen(GameScreen const&) = delete;
        GameScreen &operator=(GameScreen const&) = delete;

        void DrawUnits(Surface &frame, const CollectionData &gm1);
        void DrawTerrain(Surface &frame, const CollectionData &gm1);
        void DrawTestScene(Surface &frame);
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);

        void AdjustViewport(const SDL_Rect &screen);
    
    };
    
}

#endif
