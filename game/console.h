#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <SDL.h>

#include <sstream>
#include <deque>
#include <string>

#include <game/screen.h>
#include <game/textarea.h>

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

        void LogMessage(const std::string &message);
        bool IsClosed() const;
        void Render(Render::Renderer &renderer);
        bool HandleEvent(const SDL_Event &event);

    protected:
        ScreenManager &mScreenManager;
        std::deque<std::string> mCommandHistory;
        std::ostringstream mConsoleBuffer;
        TextArea mPromptArea;
        TextArea mLogArea;
        bool mClosed;
        
        void OnCommandEntered(const std::string &text);        
        bool HandleKey(const SDL_KeyboardEvent &event);
        bool HandleTextInput(const SDL_TextInputEvent &text);
    };
}

#endif  // CONSOLE_H_
