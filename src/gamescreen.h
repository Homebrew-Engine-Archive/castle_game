#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

class GameScreen;

#include <memory>
#include "SDL.h"
#include "rootscreen.h"
#include "screen.h"
#include "renderer.h"
#include "gamemap.h"

enum class Direction;
class Surface;

enum class CursorMode : int {
    Normal,
    Delete
};

class GameScreen : public Screen
{
    RootScreen *m_root;
    Renderer *m_renderer;
    GameMap m_map;
    int m_cursorX;
    int m_cursorY;
    bool m_cursorInvalid;
    int m_viewportX;
    int m_viewportY;
    int m_viewportRadius;
    Direction m_viewportOrient;
    bool m_flatView;
    bool m_lowView;
    bool m_zoomedOut;
    bool m_hiddenUI;
    CursorMode m_cursorMode;
    bool m_closed;
    
public:
    GameScreen(RootScreen *root);
    GameScreen(const GameScreen &that) = delete;
    GameScreen &operator=(const GameScreen &that) = delete;
    ~GameScreen();
    
    void Draw(Surface &frame);
    bool HandleEvent(const SDL_Event &event);

    // void LogWindowEvent(const SDL_WindowEvent &event);
    void AdjustViewport(const SDL_Rect &screen);
    
};

#endif
