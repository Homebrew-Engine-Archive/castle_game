#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <SDL.h>
#include <iosfwd>
#include <string>

#include <game/screen.h>

namespace Render
{
    class Renderer;
    class FontManager;
}

namespace UI
{
    class ScreenManager;
}

namespace UI
{
    class Console : public Screen
    {
    private:
        Render::Renderer &mRenderer;
        Render::FontManager &mFontManager;
        UI::ScreenManager &mScreenManager;
        
        std::string mText;
        std::string mFontName;
        int mFontSize;
        bool mClosed;

        bool HandleKey(const SDL_KeyboardEvent &event);
        bool HandleTextInput(const SDL_TextInputEvent &text);
    
    public:
        Console(Render::Renderer &renderer, Render::FontManager &fontManager, UI::ScreenManager &screenManager);
        Console(Console const&) = delete;
        Console& operator=(Console const&) = delete;

        bool IsClosed() const;
        
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };
}

#endif  // CONSOLE_H_
