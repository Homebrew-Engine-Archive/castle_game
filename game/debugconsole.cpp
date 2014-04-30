#include "debugconsole.h"

#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>

#include <game/screenmanager.h>
#include <game/sdl_utils.h>
#include <game/make_unique.h>
#include <game/surface.h>
#include <game/renderer.h>

namespace UI
{
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
        DebugConsole(DebugConsole&&) = default;
        DebugConsole &operator=(DebugConsole const&) = delete;
        DebugConsole &operator=(DebugConsole&&) = default;

        bool IsClosed() const;
        
        void Draw(Surface &frame);
        bool IsDirty(int64_t elapsed);
        bool HandleEvent(const SDL_Event &event);
    };
    
    DebugConsole::DebugConsole(ScreenManager *mgr, Render::Renderer *render)
        : mScreenMgr(mgr)
        , mRenderer(render)
        , mText("")
        , mFontName("font_stronghold_aa")
        , mFontSize(14)
        , mClosed(false)
    { }

    void DebugConsole::Draw(Surface&)
    {
    }

    bool DebugConsole::IsDirty(int64_t elapsed)
    {
        return (elapsed != 0);
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
        case SDLK_ESCAPE:
            mScreenMgr->CloseScreen(this);
            return true;
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

    ScreenPtr CreateDebugConsole(ScreenManager *mgr, Render::Renderer *render)
    {
        return std::make_unique<DebugConsole>(mgr, render);
    }

} // namespace UI
