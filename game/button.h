#ifndef BUTTON_H_
#define BUTTON_H_

#include <SDL.h>

#include <functional>

#include <game/rect.h>
#include <game/surface.h>

namespace UI
{

    enum class ButtonState : int {
        Released,
        Over,
        Pressed
    };

    class Button
    {
        Rect mBoundRect;
        Surface mReleased;
        Surface mOver;
        Surface mPressed;
        std::function<void()> mHandler;
        ButtonState mState;

        void MouseMotion(int x, int y);
        void MousePressed(int x, int y);
        void MouseReleased(int x, int y);
        void SetButtonState(ButtonState state);
    
    public:
        Button(const Rect &rect, Surface released, Surface over, Surface pressed, void handler());
    
        virtual void Draw(Surface &surface);
        virtual void HandleEvent(const SDL_Event &event);

        virtual Rect BoundRect() const;
    };

}

#endif
