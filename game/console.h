#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <SDL.h>
#include <iosfwd>
#include <string>

#include <game/screen.h>

namespace UI
{
    class ScreenManager;
}

namespace UI
{
    class Console : public Screen
    {
    private:
        UI::ScreenManager &mScreenManager;
        
        std::string mText;
        std::string mFontName;
        int mFontSize;
        bool mClosed;

        bool HandleKey(const SDL_KeyboardEvent &event);
        bool HandleTextInput(const SDL_TextInputEvent &text);
    
    public:
        Console(UI::ScreenManager &screenManager);
        Console(Console const&) = delete;
        Console& operator=(Console const&) = delete;

        bool IsClosed() const;
        
        void Draw(Surface &frame);
        bool HandleEvent(const SDL_Event &event);
    };
}

#endif  // CONSOLE_H_
