#ifndef BUTTON_H_
#define BUTTON_H_

#include <SDL.h>
#include <functional>
#include <game/surface.h>
#include <game/widget.h>

namespace UI
{

    enum class ButtonState : int {
        Released,
        Over,
        Pressed
    };

    class Button : public Widget
    {
        SDL_Rect mBoundRect;
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
        Button(const SDL_Rect &rect, Surface released, Surface over, Surface pressed, std::function<void()> handler);
    
        virtual void Draw(Surface &surface);
        virtual void HandleEvent(const SDL_Event &event);

        virtual SDL_Rect Rect() const;
    };

}

#endif
