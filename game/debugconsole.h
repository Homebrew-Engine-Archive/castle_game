#ifndef DEBUG_CONSOLE_H_
#define DEBUG_CONSOLE_H_

#include <SDL.h>
#include <iosfwd>
#include <string>

#include <game/screen.h>

namespace Castle
{
    class Engine;
}

namespace Render
{
    class Renderer;
}

namespace UI
{
    class ScreenManager;

    class DebugConsole : public Screen
    {
    private:
        ScreenManager *mScreenMgr;
        Render::Renderer *mRenderer;
        std::string mText;
        std::string mFontName;
        int mFontSize;
        bool mClosed;

        bool HandleKey(const SDL_KeyboardEvent &event);
        bool HandleTextInput(const SDL_TextInputEvent &text);
    
    public:
        DebugConsole(ScreenManager *mgr, Render::Renderer *render);
        DebugConsole(DebugConsole const&) = delete;
        DebugConsole &operator=(DebugConsole const&) = delete;

        bool IsClosed() const;
        
        void Draw(Surface &frame);
        bool IsDirty(int64_t elapsed);
        bool HandleEvent(const SDL_Event &event);
    };
}

#endif  // DEBUGCONSOLE_H_
