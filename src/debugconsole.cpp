#include "debugconsole.h"

#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>

#include "screenmanager.h"
#include "geometry.h"
#include "macrosota.h"
#include "surface.h"
#include "renderer.h"

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
        std::ostream &mSinkStream;
        std::istream &mLogStream;
        bool mClosed;

        bool HandleKey(const SDL_KeyboardEvent &event);
        bool HandleTextInput(const SDL_TextInputEvent &text);
    
    public:
        DebugConsole(ScreenManager *mgr, Render::Renderer *render, std::ostream &sink, std::istream &log);
        DebugConsole(const DebugConsole &) = delete;
        DebugConsole(DebugConsole &&) = default;
        DebugConsole &operator=(const DebugConsole &) = delete;
        DebugConsole &operator=(DebugConsole &&) = default;

        bool IsClosed() const;
        
        void Draw(Surface &frame);
        bool IsDirty(int64_t elapsed);
        bool HandleEvent(const SDL_Event &event);
    };
    
    DebugConsole::DebugConsole(ScreenManager *mgr, Render::Renderer *render, std::ostream &sink, std::istream &log)
        : mScreenMgr(mgr)
        , mRenderer(render)
        , mText("")
        , mFontName("font_stronghold_aa")
        , mFontSize(14)
        , mSinkStream(sink)
        , mLogStream(log)
        , mClosed(false)
    { }

    void DebugConsole::Draw(Surface &frame)
    {
        BlurSurface(frame, 255);
        SDL_Rect bounds = SurfaceBounds(frame);
        SDL_Rect tophalf = MakeRect(bounds.w, bounds.h / 2);
        FillFrame(frame, &tophalf, 0x7f000000);

        SDL_Rect textBox = PutIn(MakeRect(tophalf.w, mFontSize), tophalf, 0.0f, 1.0f);
        mRenderer->SetFont(mFontName, mFontSize);
        mRenderer->RenderTextLine(mText, TopLeft(textBox));
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
            mSinkStream << mText << std::endl;
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

    ScreenPtr CreateDebugConsole(ScreenManager *mgr, Render::Renderer *render, std::ostream &sink, std::istream &log)
    {
        return make_unique<DebugConsole>(mgr, render, sink, log);
    }

} // namespace UI
