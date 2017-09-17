#include "screenmanager.h"

#include <stdexcept>

#include <game/textarea.h>

namespace castle
{
    namespace ui
    {
        ScreenManager::ScreenManager()
            : mConsole(*this)
            , mMenuMain(*this)
            , mGameScreen(*this)
            , mScreenStack()
            , mInfoArea(new TextArea())
        {
            mInfoArea->SetTextColor(core::colors::Red);
            mInfoArea->SetBackgroundColor(core::colors::Black.Opaque(160));
            mInfoArea->SetMaxWidth(200);
        }

        void ScreenManager::PushScreen(Screen &screen)
        {
            mScreenStack.push_back(&screen);
        }

        Screen& ScreenManager::TopScreen()
        {
            if(mScreenStack.empty()) {
                throw std::runtime_error("ask about top of empty screen stack");
            }
            return *mScreenStack.back();
        }

        Screen& ScreenManager::PopScreen()
        {
            if(mScreenStack.empty()) {
                throw std::runtime_error("pop top of empty stack");
            }
            Screen &topScreen = TopScreen();
            mScreenStack.pop_back();
            return topScreen;
        }

        void ScreenManager::CloseScreen(Screen *screen)
        {
            auto end = std::remove(mScreenStack.begin(), mScreenStack.end(), screen);
            mScreenStack.erase(end, mScreenStack.end());
        }
    
        bool ScreenManager::HandleEvent(const SDL_Event &event)
        {
            return TopScreen().HandleEvent(event);
        }

        void ScreenManager::Render(render::Renderer &renderer)
        {
            for(Screen *screen : mScreenStack) {
                if(screen != nullptr) {
                    screen->Render(renderer);
                } else {
                    throw std::runtime_error("null screen on stack");
                }
            }
            if(&TopScreen() != &mConsole) {
                mInfoArea->Render(renderer);
            }
        }
    
        void ScreenManager::EnterGameScreen()
        {
            PushScreen(mGameScreen);
        }

        void ScreenManager::EnterMenuMain()
        {
            PushScreen(mMenuMain);
        }

        MenuMain& ScreenManager::GetMenuMain()
        {
            return mMenuMain;
        }
    
        GameScreen& ScreenManager::GetGameScreen()
        {
            return mGameScreen;
        }
    
        Console& ScreenManager::GetConsole()
        {
            return mConsole;
        }

        void ScreenManager::ToggleConsole()
        {
            if(&TopScreen() == &mConsole) {
                PopScreen();
            } else {
                PushScreen(mConsole);
            }
        }
        
        void ScreenManager::SetTestString(std::string test)
        {
            mInfoArea->SetText(std::move(test));
        }
        
    } // namespace ui
}
