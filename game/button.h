#ifndef BUTTON_H_
#define BUTTON_H_

#include <SDL.h>

#include <chrono>
#include <functional>

#include <game/rect.h>
#include <game/surface.h>

namespace UI
{
    enum class ButtonState : int
    {
        Released,
        Over,
        Pressed
    };

    class Button
    {
        Rect mBoundsRect;
        ButtonState mState;
        std::function<void(const Button &button)> mStateHandler;
        
        void MouseMotion(int x, int y);
        void MousePressed(int x, int y);
        void MouseReleased(int x, int y);
        void SetButtonState(ButtonState state);

    public:
        explicit Button(const Rect &rect);

        virtual void Update(std::chrono::milliseconds const&);
        
        virtual void Draw(Surface &surface);
        virtual void HandleEvent(const SDL_Event &event);

        virtual void SetBoundsRect(const Rect &rect);
        virtual Rect BoundsRect() const;

        virtual void WhenPressed(void clickHandler(const Button &button));
    };
}

#endif
