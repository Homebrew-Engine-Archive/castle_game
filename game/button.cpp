#include "button.h"
#include "geometry.h"

namespace UI
{

    Button::Button(const SDL_Rect &rect, Surface released, Surface over, Surface pressed, std::function<void()> handler)
        : mBoundRect(rect)
        , mReleased(released)
        , mOver(over)
        , mPressed(pressed)
        , mHandler(handler)
        , mState(ButtonState::Released)
    {
    }

    void Button::SetButtonState(ButtonState state)
    {
        mState = state;
    }

    void Button::Draw(Surface &surface)
    {
        Surface buttonSurface;
        switch(mState) {
        case ButtonState::Released:
            buttonSurface = mReleased;
            break;
        case ButtonState::Over:
            buttonSurface = mOver;
            break;
        case ButtonState::Pressed:
            buttonSurface = mPressed;
            break;
        default:
            throw std::runtime_error("Another button state?");
        }

        BlitSurface(buttonSurface, NULL, surface, &mBoundRect);
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
        if(IsInRect(mBoundRect, x, y)) {
            SetButtonState(ButtonState::Over);
        } else {
            SetButtonState(ButtonState::Released);
        }
    }

    void Button::MousePressed(int x, int y)
    {
        if(IsInRect(mBoundRect, x, y)) {
            SetButtonState(ButtonState::Pressed);
            mHandler();
        } else {
            SetButtonState(ButtonState::Released);
        }
    }

    void Button::MouseReleased(int x, int y)
    {
        if(IsInRect(mBoundRect, x, y)) {
            SetButtonState(ButtonState::Over);
        } else {
            SetButtonState(ButtonState::Released);
        }
    }

    SDL_Rect Button::Rect() const
    {
        return mBoundRect;
    }
    
} // namespace UI
