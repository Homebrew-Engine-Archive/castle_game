#include "screenmanager.h"

#include <stdexcept>

#include <game/exception.h>

namespace UI
{
    ScreenManager::ScreenManager(Render::Renderer &renderer, Render::FontManager &fontManager)
        : mRenderer(renderer)
        , mFontManager(fontManager)
        , mLoadingScreen()
        , mConsole(renderer, fontManager, *this)
        , mMenuMain(renderer, fontManager, *this)
        , mGameScreen(renderer, fontManager, *this)
        , mMenuCombat(renderer, fontManager, *this)
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
    
    UI::Console& ScreenManager::Console()
    {
        return mConsole;
    }

    UI::LoadingScreen& ScreenManager::LoadingScreen()
    {
        return mLoadingScreen;
    }
    
} // namespace UI
