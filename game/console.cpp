#include "console.h"

#include <SDL.h>

#include <iostream>
#include <string>

#include <game/renderer.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/fontmanager.h>
#include <game/screenmanager.h>
#include <game/surface.h>
#include <game/textrenderer.h>
#include <game/surface_drawing.h>

namespace UI
{
    Console::Console(UI::ScreenManager &screenManager)
        : mScreenManager(screenManager)
        , mText()
        , mFontName(Render::FontStronghold)
        , mCommandHistory()
        , mFontSize(10)
        , mClosed(false)
    { }

    void Console::Render(Render::Renderer &renderer)
    {
        const Rect consoleRect = Rect(BottomRight(renderer.GetScreenRect()) / Point(1, 2));
        renderer.FillFrame(consoleRect, Colors::Black.Opaque(200));
        
        Render::TextRenderer textRenderer = renderer.GetTextRenderer();
        textRenderer.SetColor(Colors::Gray);
        textRenderer.SetCursorPos(BottomLeft(consoleRect) - Point(0, 20));
        textRenderer.PutString(mText);
    }

    void Console::OnCommandEntered(const std::string &command)
    {
        mCommandHistory.push_back(command);
        mConsoleBuffer << "> " << command << std::endl;
    }
    
    bool Console::HandleEvent(const SDL_Event &event)
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

    bool Console::HandleKey(const SDL_KeyboardEvent &event)
    {
        switch(event.keysym.sym) {
        case SDLK_ESCAPE:
            {
                mScreenManager.ToggleConsole();
            }
            return true;
        case SDLK_RETURN:
            {
                OnCommandEntered(mText);
                mText = std::string();
            }
            return true;
        case SDLK_BACKSPACE:
            if(!mText.empty()) {
                mText.erase(mText.size() - 1);
            }
            return true;
        default:
            return false;
        }
    }

    bool Console::HandleTextInput(const SDL_TextInputEvent &text)
    {
        mText += text.text;
        return true;
    }
} // namespace UI
