#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>

#include <SDL2/SDL.h>

#include "gamemap.h"
#include "landscape.h"
#include "screen.h"
#include "renderer.h"

enum class Orient : int {
    Front,
    Left,
    Right,
    Back
};

enum class BuildingCategory : int {
    Castle,
    Industry,
    Farming,
    Weaponary,
    Civil,
    Production
};

enum class CursorMode : int {
    Normal,
    Delete
};

Orient PrevRotation(Orient rot);
Orient NextRotation(Orient rot);

class GameScreen : public Screen
{
    GameMap m_map;
    int m_cursorX;
    int m_cursorY;
    bool m_cursorInvalid;
    int m_viewportX;
    int m_viewportY;
    int m_viewportRadius;
    Orient m_viewportOrient;
    bool m_flatView;
    bool m_lowView;
    bool m_zoomedOut;
    bool m_hiddenUI;
    CursorMode m_cursorMode;
    bool m_closed;
    
public:
    GameScreen();
    ~GameScreen();
    
    void Draw(Renderer &renderer);
    bool HandleEvent(const SDL_Event &event);

    void LogWindowEvent(const SDL_WindowEvent &event);
    void AdjustViewport(const SDL_Rect &screen);
    
};

#endif
