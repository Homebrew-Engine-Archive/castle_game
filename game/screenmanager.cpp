#include "screenmanager.h"

#include <stdexcept>

namespace UI
{
    ScreenManager::ScreenManager()
        : mLoadingScreen()
        , mConsole(*this)
        , mMenuMain(*this)
        , mGameScreen(*this)
        , mMenuCombat(*this)
        , mInGameMenu()
        , mScreenStack()
    {
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

    void ScreenManager::Render(Render::Renderer &renderer)
    {
        for(UI::Screen *screen : mScreenStack) {
            if(screen != nullptr) {
                screen->Render(renderer);
            } else {
                throw std::runtime_error("null screen on stack");
            }
        }
    }
    
    void ScreenManager::DrawScreen(Surface &frame)
    {
        for(UI::Screen *screen : mScreenStack) {
            if(screen != nullptr) {
                screen->Draw(frame);
            } else {
                throw std::runtime_error("null screen on stack");
            }
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

    void ScreenManager::EnterMenuCombat()
    {
        PushScreen(mMenuCombat);
    }

    UI::MenuMain& ScreenManager::MenuMain()
    {
        return mMenuMain;
    }
    
    UI::GameScreen& ScreenManager::GameScreen()
    {
        return mGameScreen;
    }
    
    UI::Console& ScreenManager::Console()
    {
        return mConsole;
    }

    UI::LoadingScreen& ScreenManager::LoadingScreen()
    {
        return mLoadingScreen;
    }

    void ScreenManager::ToggleConsole()
    {
        if(&TopScreen() == &mConsole) {
            PopScreen();
        } else {
            PushScreen(mConsole);
        }
    }
    
} // namespace UI
