#include "screenmanager.h"

#include <stdexcept>

#include <game/exception.h>

namespace UI
{
    ScreenManager::ScreenManager(Castle::SimulationManager &simulationManager)
        : mSimulationManager(simulationManager)
        , mLoadingScreen()
        , mConsole(*this)
        , mMenuMain(*this)
        , mGameScreen(*this, simulationManager)
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
        mScreenStack.resize(
            std::remove(mScreenStack.begin(), mScreenStack.end(), screen) - mScreenStack.begin());
    }
    
    bool ScreenManager::HandleEvent(const SDL_Event &event)
    {
        return TopScreen().HandleEvent(event);
    }

    void ScreenManager::DrawScreen(Surface &frame)
    {
        for(auto &ptr : mScreenStack) {
            if(ptr) {
                ptr->Draw(frame);
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
    
} // namespace UI
