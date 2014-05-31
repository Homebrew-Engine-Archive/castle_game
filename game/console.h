#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <SDL.h>

#include <sstream>
#include <deque>
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
    public:
        Console(UI::ScreenManager &screenManager);
        Console(Console const&) = delete;
        Console& operator=(Console const&) = delete;

        bool IsClosed() const;
        void Render(Render::Renderer &renderer);
        bool HandleEvent(const SDL_Event &event);

    protected:
        UI::ScreenManager &mScreenManager;
        std::string mText;
        std::string mFontName;
        std::deque<std::string> mCommandHistory;
        std::ostringstream mConsoleBuffer;
        int mFontSize;
        bool mClosed;

        void OnCommandEntered(const std::string &text);
        
        bool HandleKey(const SDL_KeyboardEvent &event);
        bool HandleTextInput(const SDL_TextInputEvent &text);
    };
}

#endif  // CONSOLE_H_
