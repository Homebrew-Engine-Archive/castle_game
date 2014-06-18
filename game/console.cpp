#include "console.h"

#include <csignal>
#include <SDL.h>

#include <cassert>
#include <iostream>
#include <string>

#include <game/font.h>
#include <game/alignment.h>
#include <game/renderer.h>
#include <game/color.h>
#include <game/rect.h>
#include <game/fontengine.h>
#include <game/screenmanager.h>
#include <game/image.h>
#include <game/point.h>

namespace Castle
{
    namespace UI
    {
        Console::Console(ScreenManager &screenManager)
            : mScreenManager(screenManager)
            , mCommandHistory()
            , mConsoleBuffer()
            , mPromptArea()
            , mLogArea()
        {
            using namespace core::colors;
            mPromptArea.SetAlignment(core::Alignment::Expanded, core::Alignment::Max);
            mPromptArea.SetTextColor(Magenta);
            mPromptArea.SetBackgroundColor(Blue.Opaque(50));
            mPromptArea.SetTextAlignment(core::Alignment::Min);

            mLogArea.SetAlignment(core::Alignment::Expanded, core::Alignment::Min);
            mLogArea.SetTextColor(Gray);
            mLogArea.SetBackgroundColor(Black.Opaque(0));
            mLogArea.SetTextAlignment(core::Alignment::Min);
        }

        void Console::Render(Render::Renderer &renderer)
        {
            const core::Rect consoleRect = core::Rect(core::BottomRight(renderer.GetScreenRect()) / core::Point(1, 2));
            renderer.SetDrawColor(core::colors::Black.Opaque(200));
            renderer.FillFrame(consoleRect);
            renderer.SetViewport(consoleRect);

            mPromptArea.SetMaxWidth(consoleRect.w);
            const core::Rect promptRect = mPromptArea.FitToScreen(renderer);
        
            renderer.SetViewport(ChopBottom(renderer.GetScreenRect(), promptRect.h));
            mLogArea.SetMaxWidth(consoleRect.w);
            
            
            mLogArea.Render(renderer);
            renderer.RestoreViewport();

            mPromptArea.Render(renderer);
            renderer.RestoreViewport();
        }

        void Console::OnCommandEntered(const std::string &command)
        {
            mCommandHistory.push_back(command);
            mConsoleBuffer << "> " << command << std::endl;
            mLogArea.SetText(mConsoleBuffer.str());
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
                    OnCommandEntered(mPromptArea.Text());
                    mPromptArea.SetText(std::string());
                }
                return true;
            case SDLK_BACKSPACE:
                {
                    std::string text = mPromptArea.Text();
                    if(!text.empty()) {
                        text.pop_back();
                        mPromptArea.SetText(text);
                    }
                }
                return true;
            default:
                return false;
            }
        }

        void Console::LogMessage(const std::string &message)
        {
            //mConsoleBuffer << message << std::endl;
        }
    
        bool Console::HandleTextInput(const SDL_TextInputEvent &event)
        {
            mPromptArea.SetText(mPromptArea.Text() + event.text);
            return true;
        }
    } // namespace UI
}
