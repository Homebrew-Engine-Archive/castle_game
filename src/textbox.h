#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "widget.h"

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
    };

}

#endif
