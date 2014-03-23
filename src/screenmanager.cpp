#include "screenmanager.h"

#include <stdexcept>

#include "debugconsole.h"
#include "macrosota.h"

namespace GUI
{

    ScreenManager::ScreenManager(Castle::Engine *engine)
        : mEngine(engine)
        , mConsole(std::move(GUI::CreateDebugConsole(engine)))
        , mShowConsole(false)
        , mScreenStack()
    {
    }

    void ScreenManager::PushScreen(ScreenPtr &&screen)
    {
        mScreenStack.push_back(std::move(screen));
    }

    Screen *ScreenManager::GetCurrentScreen()
    {
        if(mScreenStack.empty()) {
            return nullptr;
        }
        return mScreenStack.back().get();
    }

    ScreenPtr ScreenManager::PopScreen()
    {
        if(mScreenStack.empty()) {
            return ScreenPtr(nullptr);
        }
        ScreenPtr ptr = std::move(mScreenStack.back());
        mScreenStack.pop_back();
        return std::move(ptr);
    }
    
    void ScreenManager::SetCurrentScreen(ScreenPtr &&screen)
    {
        mScreenStack.clear();
        mScreenStack.push_back(std::move(screen));
    }

    ScreenPtr ScreenManager::RemoveScreen(Screen *screen)
    {
        ScreenPtr removed;

        size_t index = 0;
        for(ScreenPtr &ptr : mScreenStack) {
            if(ptr.get() == screen) {
                removed = std::move(ptr);
                break;
            }
            ++index;
        }

        if(index < mScreenStack.size()) {
            mScreenStack.erase(mScreenStack.begin() + index);
        }
        
        return removed;
    }
    
    void ScreenManager::ShowConsole(bool show)
    {
        if(mShowConsole != show) {
            if(show) {
                PushScreen(std::move(mConsole));
            } else {
                mConsole = std::move(PopScreen());
            }
            mShowConsole = show;
        }        
    }
    
    bool ScreenManager::HandleEvent(const SDL_Event &event)
    {
        Screen *top = GetCurrentScreen();
        if(top == nullptr)
            return false;

        return top->HandleEvent(event);
    }

    void ScreenManager::DrawScreen(Surface &frame)
    {
        for(GUI::ScreenPtr &ptr : mScreenStack) {
            if(ptr) {
                ptr->Draw(frame);
            } else {
                // NOTE
                // It's assumed to be impossible
                throw std::runtime_error("empty screen in screen stack");
            }
        }
    }
    
}
