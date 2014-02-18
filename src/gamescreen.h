#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include <memory>

#include <SDL2/SDL.h>

#include "gamemap.h"
#include "landscape.h"
#include "screen.h"
#include "renderer.h"

enum class Zoom : uint32_t {
    Normal,
    Large
};

enum class Orient : uint32_t {
    Front,
    Left,
    Right,
    Back
};

Orient PrevRotation(Orient rot);
Orient NextRotation(Orient rot);

class GameScreen : public Screen
{
    GameMap m_map;
    Uint32 m_cursorX;
    Uint32 m_cursorY;
    bool m_cursorInvalid;
    SDL_Rect m_viewport;
    Orient m_viewportOrient;
    Zoom m_viewportZoom;
    
public:
    GameScreen();
    ~GameScreen();
    
    void OnFrame(Renderer &renderer);
    void OnEvent(const SDL_Event &event);
    void OnEnterEventLoop();

    void HandleWindowEvent(const SDL_WindowEvent &event);
    bool SlideViewport(int dx, int dy);
    
    bool Closed() const;
    
    std::unique_ptr<Screen> NextScreen();
};

#endif
