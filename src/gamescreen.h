#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

class GameScreen;

#include <memory>
#include <SDL.h>
#include "engine.h"
#include "screen.h"
#include "renderer.h"
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

class GameScreen : public Screen
{
    Castle::Engine *mRoot;
    Renderer *mRenderer;
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
    GameScreen(Castle::Engine *root);
    GameScreen(const GameScreen &that) = delete;
    GameScreen &operator=(const GameScreen &that) = delete;
    ~GameScreen();
    
    void Draw(Surface &frame);
    bool HandleEvent(const SDL_Event &event);

    // void LogWindowEvent(const SDL_WindowEvent &event);
    void AdjustViewport(const SDL_Rect &screen);
    
};

#endif
