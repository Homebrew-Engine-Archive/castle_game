#include "debugconsole.h"

#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>

#include "geometry.h"
#include "macrosota.h"
#include "engine.h"
#include "surface.h"
#include "renderer.h"

namespace GUI
{
    
    class DebugConsole : public Screen
    {
    private:
        Castle::Engine *mRoot;
        std::string mText;
        std::string mFontName;
        int mFontSize;

        bool HandleKey(const SDL_KeyboardEvent &event);
        bool HandleTextInput(const SDL_TextInputEvent &text);
    
    public:
        DebugConsole(Castle::Engine *root);
        DebugConsole(const DebugConsole &) = delete;
        DebugConsole(DebugConsole &&) = default;
        DebugConsole &operator=(const DebugConsole &) = delete;
        DebugConsole &operator=(DebugConsole &&) = default;
    
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };


    DebugConsole::DebugConsole(Castle::Engine *root)
        : mRoot(root)
        , mText("")
        , mFontName("font_stronghold_aa")
        , mFontSize(14)
    { }

    void DebugConsole::Draw(Surface &frame)
    {
        BlurSurface(frame, 5);
        SDL_Rect bounds = SurfaceBounds(frame);
        SDL_Rect tophalf = MakeRect(bounds.w, bounds.h / 2);
        FillFrame(frame, &tophalf, 0x7f000000);

        SDL_Rect textBox = PutIn(MakeRect(tophalf.w, mFontSize), tophalf, 0.0f, 1.0f);
        Renderer *renderer = mRoot->GetRenderer();
        renderer->SetFont(mFontName, mFontSize);
        renderer->RenderTextLine(mText, BottomLeft(textBox));
    }

    bool DebugConsole::HandleEvent(const SDL_Event &event)
    {
        switch(event.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            return HandleKey(event.key);
        case SDL_TEXTINPUT:
            return HandleTextInput(event.text);
        default:
            return false;
        }
    }

    bool DebugConsole::HandleKey(const SDL_KeyboardEvent &event)
    {
        switch(event.keysym.sym) {
        case SDLK_RETURN:
            mText = std::string();
            return true;
        case SDLK_BACKSPACE:
            if(mText.size() > 0)
                mText.erase(mText.size() - 1);
            return true;
        default:
            return false;
        }
    }

    bool DebugConsole::HandleTextInput(const SDL_TextInputEvent &text)
    {
        mText += text.text;
        return true;
    }

    ScreenPtr CreateDebugConsole(Castle::Engine *root)
    {
        return make_unique<DebugConsole>(root);
    }

} // namespace GUI
