#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>
#include <SDL.h>
#include "screen.h"
#include "gamemap.h"

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
}

namespace GUI
{

    class GameScreen : public Screen
    {
        Castle::Engine *mEngine;
        Render::Renderer *mRenderer;
        GameMap mMap;
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
    
    public:
        GameScreen(Castle::Engine *engine);
        GameScreen(const GameScreen &that) = delete;
        GameScreen &operator=(const GameScreen &that) = delete;
        ~GameScreen();
    
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);

        // void LogWindowEvent(const SDL_WindowEvent &event);
        void AdjustViewport(const SDL_Rect &screen);
    
    };

    std::unique_ptr<GameScreen> CreateGameScreen(Castle::Engine *engine);
    
}

#endif
