#ifndef BUTTON_H_
#define BUTTON_H_

#include "SDL.h"
#include <functional>
#include "geometry.h"
#include "surface.h"

enum class ButtonState : int {
    Released,
    Over,
    Pressed
};

class Button
{
    SDL_Rect m_boundRect;
    Surface m_released;
    Surface m_over;
    Surface m_pressed;
    std::function<void()> m_handler;
    ButtonState m_state;

    void MouseMotion(int x, int y);
    void MousePressed(int x, int y);
    void MouseReleased(int x, int y);
    void SetButtonState(ButtonState state);
    
public:
    Button(const SDL_Rect &rect,
           Surface released,
           Surface over,
           Surface pressed,
           std::function<void()> handler);

    virtual SDL_Rect GetDrawingRect(int xoff, int yoff);
    
    virtual void Draw(Surface surface);
    virtual void HandleEvent(const SDL_Event &event);
    
};

#endif
