#include "gamescreen.h"

GameScreen::GameScreen()
    : m_cursorX(0)
    , m_cursorY(0)
    , m_cursorInvalid(true)
    , m_viewportX(0)
    , m_viewportY(0)
    , m_viewportRadius(1)
    , m_viewportOrient(Orient::Front)
    , m_flatView(false)
    , m_lowView(false)
    , m_zoomedOut(false)
    , m_hiddenUI(false)
    , m_cursorMode(CursorMode::Normal)
{
}

GameScreen::~GameScreen()
{
}

void GameScreen::Draw(Renderer &renderer)
{
    if(!m_cursorInvalid) {
        AdjustViewport(renderer.OutputRect());
    }

    renderer.Clear();
    renderer.Present();
}

bool GameScreen::HandleEvent(const SDL_Event &event)
{
    switch(event.type) {
    case SDL_WINDOWEVENT:
        {
            HandleWindowEvent(event.window);
        }
        break;
    case SDL_MOUSEMOTION:
        {
            m_cursorInvalid = false;
            m_cursorX = event.motion.x;
            m_cursorY = event.motion.y;
        }
        break;
    case SDL_KEYDOWN:
        {
            m_closed = true;
        }
        break;
    }
    return true;
}

void GameScreen::AdjustViewport(const SDL_Rect &screen)
{
    if(screen.w == m_cursorX)
        ++m_viewportX;
    else if(0 == m_cursorX)
        --m_viewportX;

    if(screen.h == m_cursorY)
        ++m_viewportY;
    else if(0 == m_cursorY)
        --m_viewportY;
}

void GameScreen::HandleWindowEvent(const SDL_WindowEvent &window)
{
    switch (window.event) {
    case SDL_WINDOWEVENT_SHOWN:
        SDL_Log("Window %d shown", window.windowID);
        break;
    case SDL_WINDOWEVENT_HIDDEN:
        SDL_Log("Window %d hidden", window.windowID);
        break;
    case SDL_WINDOWEVENT_EXPOSED:
        SDL_Log("Window %d exposed", window.windowID);
        break;
    case SDL_WINDOWEVENT_MOVED:
        SDL_Log("Window %d moved to %d,%d",
                window.windowID, window.data1,
                window.data2);
        break;
    case SDL_WINDOWEVENT_RESIZED:
        SDL_Log("Window %d resized to %dx%d",
                window.windowID, window.data1,
                window.data2);
        break;
    case SDL_WINDOWEVENT_MINIMIZED:
        SDL_Log("Window %d minimized", window.windowID);
        break;
    case SDL_WINDOWEVENT_MAXIMIZED:
        SDL_Log("Window %d maximized", window.windowID);
        break;
    case SDL_WINDOWEVENT_RESTORED:
        SDL_Log("Window %d restored", window.windowID);
        break;
    case SDL_WINDOWEVENT_ENTER:
        SDL_Log("Mouse entered window %d",
                window.windowID);
        break;
    case SDL_WINDOWEVENT_LEAVE:
        SDL_Log("Mouse left window %d", window.windowID);
        break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        SDL_Log("Window %d gained keyboard focus",
                window.windowID);
        break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        SDL_Log("Window %d lost keyboard focus",
                window.windowID);
        break;
    case SDL_WINDOWEVENT_CLOSE:
        SDL_Log("Window %d closed", window.windowID);
        break;
    default:
        SDL_Log("Window %d got unknown event %d",
                window.windowID, window.event);
        break;
    }
}

Orient NextRotation(Orient rot)
{
    switch(rot) {
    case Orient::Front: return Orient::Left;
    case Orient::Left: return Orient::Back;
    case Orient::Back: return Orient::Right;
    case Orient::Right: return Orient::Front;
    default: return Orient::Front;
    }
}

Orient PrevRotation(Orient rot)
{
    switch(rot) {
    case Orient::Left: return Orient::Front;
    case Orient::Front: return Orient::Right;
    case Orient::Right: return Orient::Back;
    case Orient::Back: return Orient::Left;
    default: return Orient::Front;
    }
}
