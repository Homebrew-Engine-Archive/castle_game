#include "screenmanager.h"

#include <stdexcept>

#include <game/exception.h>
#include <game/debugconsole.h>
#include <game/make_unique.h>

namespace UI
{

    ScreenManager::ScreenManager(Render::Renderer *renderer)
        : mRenderer(renderer)
        , mConsole(this, renderer)
        , mMenuMain(this, renderer)
        , mGameScreen(this, renderer)
        , mMenuCombat(this, renderer)
        , mScreenStack()
    {
    }

    void ScreenManager::PushScreen(Screen *screen)
    {
        mScreenStack.push_back(screen);
    }

    Screen* ScreenManager::TopScreen()
    {
        if(mScreenStack.empty()) {
            throw Castle::Exception("Ask for top of empty stack", BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }
        return mScreenStack.back();
    }

    Screen* ScreenManager::PopScreen()
    {
        if(mScreenStack.empty()) {
            throw Castle::Exception("Pop top of empty stack", BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }
        Screen *topScreen = TopScreen();
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
        return TopScreen()->HandleEvent(event);
    }

    void ScreenManager::DrawScreen(Surface &frame)
    {
        for(auto &ptr : mScreenStack) {
            if(ptr) {
                ptr->Draw(frame);
            } else {
                throw Castle::Exception("Empty screen on stack", BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
            }
        }
    }

    void ScreenManager::EnterGameScreen()
    {
        PushScreen(&mGameScreen);
    }

    void ScreenManager::EnterMenuMain()
    {
        PushScreen(&mMenuMain);
    }

    void ScreenManager::EnterMenuCombat()
    {
        PushScreen(&mMenuCombat);
    }

    UI::MenuMain& ScreenManager::MenuMain()
    {
        return mMenuMain;
    }
    
    UI::GameScreen& ScreenManager::GameScreen()
    {
        return mGameScreen;
    }
    
    UI::DebugConsole& ScreenManager::DebugConsole()
    {
        return mConsole;
    }
    
} // namespace UI
