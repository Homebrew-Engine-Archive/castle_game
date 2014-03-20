#include "rootscreen.h"
#include "menu_combat.h"
#include "menu_main.h"
#include "loadingscreen.h"
#include "macrosota.h"
#include "network.h"
#include <sstream>
#include <string>
#include <boost/asio/io_service.hpp>
#include <vector>
#include "debugconsole.h"

using namespace std;
    
class RootScreenImpl final : public RootScreen
{
    Renderer *mRenderer;
    double mFpsAverage;
    uint64_t mFrameCounter;
    bool mClosed;
    int mFrameRate;
    bool mFpsLimited;
    bool mShowConsole;
    ScreenPtr mDebugConsole;
    vector<ScreenPtr> mScreenStack;
        
    bool HandleWindowEvent(const SDL_WindowEvent &event);
    bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
    Screen *GetCurrentScreen() const;
    void ToggleConsole();
    
public:
    RootScreenImpl(Renderer *mRenderer);
    RootScreenImpl(const RootScreenImpl &) = delete;
    RootScreenImpl(RootScreenImpl &&) = default;
    RootScreenImpl &operator=(const RootScreenImpl &) = delete;
    RootScreenImpl &operator=(RootScreenImpl &&) = default;
    
    int Exec();
    void DrawFrame();    
    bool HandleEvent(const SDL_Event &event);
    void SetCurrentScreen(ScreenPtr &&screen);
    void PushScreen(ScreenPtr &&screen);
    ScreenPtr PopScreen();
    Renderer *GetRenderer();

};

RootScreenImpl::RootScreenImpl(Renderer *renderer)
    : mRenderer(renderer)
    , mFpsAverage(0.0f)
    , mFrameCounter(0.0f)
    , mClosed(false)
    , mFrameRate(16)
    , mFpsLimited(true)
    , mShowConsole(false)
    , mDebugConsole(std::move(CreateDebugConsole(this)))
{ }

bool RootScreenImpl::HandleWindowEvent(const SDL_WindowEvent &window)
{
    switch(window.event) {
    case SDL_WINDOWEVENT_RESIZED:
        {
            int width = window.data1;
            int height = window.data2;
            mRenderer->AdjustBufferSize(width, height);
        }
        return true;
    default:
        return true;
    }
}

bool RootScreenImpl::HandleKeyboardEvent(const SDL_KeyboardEvent &key)
{
    switch(key.keysym.sym) {
    case SDLK_ESCAPE:
        mClosed = true;
        return false;
    case SDLK_BACKSLASH:
        ToggleConsole();
        return true;
    default:
        return true;
    }
}

void RootScreenImpl::ToggleConsole()
{
    mShowConsole = !m_showConsole;

    if(mShowConsole) {
        PushScreen(move(mDebugConsole));
    } else {
        mDebugConsole = move(PopScreen());
    }
}

Screen *RootScreenImpl::GetCurrentScreen() const
{
    if(mScreenStack.empty())
        return NULL;
    else
        return mScreenStack.back().get();
}

void RootScreenImpl::SetCurrentScreen(ScreenPtr &&screen)
{
    mScreenStack.clear();
    PushScreen(move(screen));
}

void RootScreenImpl::PushScreen(ScreenPtr &&screen)
{
    mScreenStack.push_back(move(screen));
}

ScreenPtr RootScreenImpl::PopScreen()
{
    if(!mScreenStack.empty()) {
        ScreenPtr ptr = std::move(mScreenStack.back());
        mScreenStack.pop_back();
        return ptr;
    }

    return ScreenPtr(nullptr);
}

int RootScreenImpl::Exec()
{
    if(int code = RunLoadingScreen(this)) {
        clog << "Loading has been interrupted."
             << endl;
        return code;
    }

    PushScreen(make_unique<MenuMain>(this));

    const int64_t msPerSec = 1000;
    const int64_t pollRate = 66;
    const int64_t pollInterval = msPerSec / pollRate;
    
    int64_t lastFrame = 0;
    int64_t lastPoll = 0;
    int64_t lastSecond = 0;
    int64_t fpsCounterLastSecond = 0;
    
    boost::asio::io_service io;

    const short port = 4500;
    Server server(io, port);
    server.StartAccept();
    
    while(!mClosed) {
        const int64_t frameInterval = msPerSec / mFrameRate;
        const int64_t pollStart = SDL_GetTicks();
        if(lastPoll + pollInterval < pollStart) {
            io.poll();
            lastPoll = pollStart;
        }

        if(GetCurrentScreen() == NULL) {
            throw runtime_error("No current screen");
        }
        
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!GetCurrentScreen()->HandleEvent(event))
                HandleEvent(event);
        }

        const int64_t frameStart = SDL_GetTicks();
        if(lastSecond + msPerSec < frameStart) {
            double fps = mFrameCounter - fpsCounterLastSecond;
            double elapsed = frameStart - lastSecond;
            mFpsAverage = fps * (msPerSec / elapsed);
            lastSecond = frameStart;
            fpsCounterLastSecond = mFrameCounter;
        }

        if(lastFrame + frameInterval < frameStart) {
            DrawFrame();
            lastFrame = frameStart;
            ++mFrameCounter;
        }

        const int64_t delayStart = SDL_GetTicks();
        if(mFpsLimited) {
            const int64_t nextTick =
                min(lastPoll + pollInterval,
                         lastFrame + frameInterval);
            if(delayStart < nextTick) {
                SDL_Delay(nextTick - delayStart);
            }
        }
    }

    clog << "Gracefully shutdown" << endl;
    
    return 0;
}

void RootScreenImpl::DrawFrame()
{
    Surface frame = mRenderer->BeginFrame();

    for(ScreenPtr &ptr : mScreenStack) {
        if(ptr) {
            ptr->Draw(frame);
        } else {
            // NOTE
            // It's assumed to be impossible
            throw runtime_error("empty screen in screen stack");
        }
    }
    
    int size = 24;
    string fontname = "font_stronghold_aa";
    SDL_Color color = MakeColor(255, 255, 255, 255);
    mRenderer->SetColor(color);
    mRenderer->SetFont(fontname, size);

    SDL_Point pos = ShiftPoint(TopLeft(mRenderer->GetOutputSize()), 5, 5);
    
    ostringstream oss;
    oss << "FPS: " << mFpsAverage;
    mRenderer->RenderTextLine(oss.str(), pos);

    mRenderer->EndFrame();
}

bool RootScreenImpl::HandleEvent(const SDL_Event &event)
{
    switch(event.type) {
    case SDL_WINDOWEVENT:
        return HandleWindowEvent(event.window);
    case SDL_QUIT:
        mClosed = true;
        return false;
    case SDL_KEYDOWN:
        return HandleKeyboardEvent(event.key);
    default:
        return true;
    }
}

Renderer *RootScreenImpl::GetRenderer()
{
    return mRenderer;
}

std::unique_ptr<RootScreen> CreateRootScreen(Renderer *renderer)
{
    return make_unique<RootScreenImpl>(renderer);
}

