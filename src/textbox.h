#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "widget.h"

class Text;
class RootScreen;

class TextBox : public Widget
{
public:
    TextBox(RootScreen *root);
    void SetText(const Text &text);
};


#endif
