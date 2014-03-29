#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "widget.h"
#include <SDL.h>

class Surface;
class Text;

namespace Castle
{
    class Engine;
}

namespace UI
{

    class TextBox : public Widget
    {
    public:
        TextBox(Castle::Engine *engine);
        void SetText(const Text &text);
        void HandleEvent(const SDL_Event &event);
        void Draw(Surface &surface);
        SDL_Rect Rect() const;
    };

}

#endif
