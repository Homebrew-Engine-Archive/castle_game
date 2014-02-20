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
    Uint32 m_cursorX;
    Uint32 m_cursorY;
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
    
public:
    GameScreen();
    ~GameScreen();
    
    void OnFrame(Renderer &renderer);
    void OnEvent(const SDL_Event &event);
    void OnEnterEventLoop();

    void HandleWindowEvent(const SDL_WindowEvent &event);
    void SlideViewport(int dx, int dy);
    
    bool Closed() const;
    
    std::unique_ptr<Screen> NextScreen();
};

#endif
