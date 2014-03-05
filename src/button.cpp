#include "button.h"

Button::Button(
    const SDL_Rect &rect,
    Surface released,
    Surface over,
    Surface pressed,
    std::function<void()> handler)
    : m_boundRect(rect)
    , m_released(released)
    , m_over(over)
    , m_pressed(pressed)
    , m_handler(handler)
    , m_state(ButtonState::Released)
{
}

void Button::SetButtonState(ButtonState state)
{
    m_state = state;
}

void Button::Draw(Surface surface)
{
    Surface buttonSurface;
    switch(m_state) {
    case ButtonState::Released:
        buttonSurface = m_released;
        break;
    case ButtonState::Over:
        buttonSurface = m_over;
        break;
    case ButtonState::Pressed:
        buttonSurface = m_pressed;
        break;
    default:
        throw std::runtime_error("Another button state?");
    }
    BlitSurface(buttonSurface, NULL, surface, &m_boundRect);
}

void Button::HandleEvent(const SDL_Event &event)
{
    switch(event.type) {
    case SDL_MOUSEMOTION:
        MouseMotion(event.motion.x,
                    event.motion.y);
        break;
    case SDL_MOUSEBUTTONDOWN:
        MousePressed(event.button.x,
                     event.button.y);
        break;
    case SDL_MOUSEBUTTONUP:
        MouseReleased(event.button.x,
                      event.button.y);
        break;
    default:
        break;
    }
}

void Button::MouseMotion(int x, int y)
{
    if(IsInRect(m_boundRect, x, y)) {
        SetButtonState(ButtonState::Over);
    } else {
        SetButtonState(ButtonState::Released);
    }
}

void Button::MousePressed(int x, int y)
{
    if(IsInRect(m_boundRect, x, y)) {
        SetButtonState(ButtonState::Pressed);
        m_handler();
    } else {
        SetButtonState(ButtonState::Released);
    }
}

void Button::MouseReleased(int x, int y)
{
    if(IsInRect(m_boundRect, x, y)) {
        SetButtonState(ButtonState::Over);
    } else {
        SetButtonState(ButtonState::Released);
    }
}
