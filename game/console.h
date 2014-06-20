#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <sstream>
#include <deque>
#include <string>

#include <SDL.h>

#include <game/screen.h>
#include <game/textarea.h>

namespace castle
{
    namespace render
    {
        class renderer;
    }
    
    namespace ui
    {
        class ScreenManager;
    }

    namespace ui
    {
        class Console : public Screen
        {
        public:
            explicit Console(ScreenManager &screenManager);
            Console(Console const&) = delete;
            Console& operator=(Console const&) = delete;
            virtual ~Console() = default;

            void LogMessage(const std::string &message);
            void Render(render::Renderer &renderer);
            bool HandleEvent(const SDL_Event &event);

        private:
            ScreenManager &mScreenManager;
            std::deque<std::string> mCommandHistory;
            std::ostringstream mConsoleBuffer;
            TextArea mPromptArea;
            TextArea mLogArea;

        protected:
            void OnCommandEntered(const std::string &text);        
            bool HandleKey(const SDL_KeyboardEvent &event);
            bool HandleTextInput(const SDL_TextInputEvent &text);
        };
    }
}

#endif  // CONSOLE_H_
