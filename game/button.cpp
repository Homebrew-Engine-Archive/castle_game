#include "button.h"

#include <stdexcept>

#include <game/color.h>
#include <game/fontmanager.h>
#include <game/textrenderer.h>

namespace
{
    std::string GetButtonStateName(const ButtonState &state)
    {
        switch(state) {
        case ButtonState::Released:
            return "released";
        case ButtonState::Over:
            return "over";
        case ButtonState::Pressed:
            return "pressed";
        default:
            throw std::runtime_error("wrong button state");
        }
    }
}

namespace UI
{

    Button::Button(const Rect &rect)
        : mBoundsRect(rect)
        , mState(ButtonState::Released)
        , mStateHandler([](){})
    {
    }

    void Button::Update(const std::chrono::milliseconds &delta)
    {
        
    }
    
    void Button::Draw(Surface &frame)
    {
        FillFrame(frame, mBoundsRect, Colors::Black.Opaque(100));

        const std::string stateName = GetButtonStateName(mState);
        Render::TextRenderer renderer(frame);
        renderer.SetFont(Render::FontManager::Instance().DefaultFont());
        renderer.SetColor(Colors::Red);

        const Rect textRect = renderer.CalculateTextRect(stateName);
        const Rect textAligned = PutIn(textRect, Rect(frame), 0, 0);
        renderer.Translate(textAligned.x, textAligned.y);
        renderer.PutString(stateName);
    }

    void Button::HandleEvent(const SDL_Event &event)
    {
        switch(event.type) {
        case SDL_MOUSEMOTION:
            MouseMotion(event.motion.x, event.motion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            MousePressed(event.button.x, event.button.y);
            break;
        case SDL_MOUSEBUTTONUP:
            MouseReleased(event.button.x, event.button.y);
            break;
        default:
            break;
        }
    }

    void Button::MouseMotion(int x, int y)
    {
        if(PointInRect(mBoundsRect, x, y)) {
            SetButtonState(ButtonState::Over);
        } else {
            SetButtonState(ButtonState::Released);
        }
    }

    void Button::MousePressed(int x, int y)
    {
        if(PointInRect(mBoundsRect, x, y)) {
            SetButtonState(ButtonState::Pressed);
            mStateHandler();
        } else {
            SetButtonState(ButtonState::Released);
        }
    }

    void Button::MouseReleased(int x, int y)
    {
        if(PointInRect(mBoundsRect, x, y)) {
            SetButtonState(ButtonState::Over);
        } else {
            SetButtonState(ButtonState::Released);
        }
    }

    void Button::SetButtonState(ButtonState state)
    {
        mState = state;
    }
    
    void Button::SetBoundsRect(const Rect &rect)
    {
        mBoundsRect;
    }
    
    Rect Button::BoundRect() const
    {
        return mBoundsRect;
    }

    void Button::WhenPressed(void clickHandler(const Button &button))
    {
        mStateHandler = clickHandler;
    }
    
} // namespace UI
