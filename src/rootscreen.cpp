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

struct RootScreenPimpl
{
    RootScreen *root;
    Renderer *renderer;
    double fpsAverage;
    uint64_t frameCounter;
    bool closed;
    int frameRate;
    bool fpsLimited;
    bool showConsole;
    ScreenPtr debugConsole;
    vector<ScreenPtr> screenStack;

    bool HandleEvent(const SDL_Event &event);
    bool HandleWindowEvent(const SDL_WindowEvent &event);
    bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
    void SetCurrentScreen(ScreenPtr &&screen);
    void PushScreen(ScreenPtr &&screen);
    ScreenPtr PopScreen();
    Screen *GetCurrentScreen() const;
    void ToggleConsole();
    void DrawFrame();
    int Exec();
};

bool RootScreenPimpl::HandleWindowEvent(const SDL_WindowEvent &window)
{
    switch(window.event) {
    case SDL_WINDOWEVENT_RESIZED:
        {
            int width = window.data1;
            int height = window.data2;
            renderer->AdjustBufferSize(width, height);
        }
        return true;
    default:
        return true;
    }
}

bool RootScreenPimpl::HandleKeyboardEvent(const SDL_KeyboardEvent &key)
{
    switch(key.keysym.sym) {
    case SDLK_ESCAPE:
        closed = true;
        return false;
    case SDLK_BACKSLASH:
        ToggleConsole();
        return true;
    default:
        return true;
    }
}

void RootScreenPimpl::ToggleConsole()
{
    showConsole = !showConsole;

    if(showConsole) {
        PushScreen(move(debugConsole));
    } else {
        debugConsole = move(PopScreen());
    }
}

Screen *RootScreenPimpl::GetCurrentScreen() const
{
    if(screenStack.empty())
        return NULL;
    else
        return screenStack.back().get();
}

void RootScreenPimpl::SetCurrentScreen(ScreenPtr &&screen)
{
    screenStack.clear();
    PushScreen(move(screen));
}

void RootScreenPimpl::PushScreen(ScreenPtr &&screen)
{
    screenStack.push_back(move(screen));
}

ScreenPtr RootScreenPimpl::PopScreen()
{
    if(!screenStack.empty()) {
        ScreenPtr ptr = std::move(screenStack.back());
        screenStack.pop_back();
        return ptr;
    }

    return ScreenPtr(nullptr);
}

int RootScreenPimpl::Exec()
{
    if(int code = RunLoadingScreen(root)) {
        clog << "Loading has been interrupted."
                  << endl;
        return code;
    }

    PushScreen(make_unique<MenuMain>(root));

    const int64_t msPerSec = 1000;
    const int pollRate = 66;
    const int pollInterval = msPerSec / pollRate;
    
    int64_t lastFrame = 0;
    int64_t lastPoll = 0;
    int64_t lastSecond = 0;
    int64_t fpsCounterLastSecond = 0;
    
    boost::asio::io_service io;

    const short port = 4500;
    Server server(io, port);
    server.StartAccept();
    
    while(!closed) {
        const int64_t frameInterval = msPerSec / frameRate;
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
            double fps = frameCounter - fpsCounterLastSecond;
            double elapsed = frameStart - lastSecond;
            fpsAverage = fps * (msPerSec / elapsed);
            lastSecond = frameStart;
            fpsCounterLastSecond = frameCounter;
        }

        if(lastFrame + frameInterval < frameStart) {
            DrawFrame();
            lastFrame = frameStart;
            ++frameCounter;
        }

        const int64_t delayStart = SDL_GetTicks();
        if(fpsLimited) {
            const int64_t nextTick =
                min(lastPoll + pollInterval,
                         lastFrame + frameInterval);
            if(delayStart < nextTick) {
                SDL_Delay(nextTick - delayStart);
            }
        }
    }
    
    return 0;
}

void RootScreenPimpl::DrawFrame()
{
    Surface frame = renderer->BeginFrame();

    for(ScreenPtr &ptr : screenStack) {
        if(ptr) {
            ptr->Draw(frame);
        } else {
            // NOTE
            // It's assumed to be impossible
            throw runtime_error("empty screen in screen stack");
        }
    }
    
    font_size_t size = 24;
    string fontname = "font_stronghold_aa";
    SDL_Color color = MakeColor(255, 255, 255, 255);
    renderer->SetColor(color);
    renderer->SetFont(fontname, size);

    SDL_Point pos = ShiftPoint(TopLeft(renderer->GetOutputSize()), 5, 5);
    
    ostringstream oss;
    oss << "FPS: " << fpsAverage;
    renderer->RenderTextLine(oss.str(), pos);

    renderer->EndFrame();
}

bool RootScreenPimpl::HandleEvent(const SDL_Event &event)
{
    switch(event.type) {
    case SDL_WINDOWEVENT:
        return HandleWindowEvent(event.window);
    case SDL_QUIT:
        closed = true;
        return false;
    case SDL_KEYDOWN:
        return HandleKeyboardEvent(event.key);
    default:
        return true;
    }
}

/**
 * Root screen wrapper functions
 */
RootScreen::RootScreen(Renderer *renderer)
    : m(new RootScreenPimpl)
{
    m->debugConsole = make_unique<DebugConsole>(this);
    m->root = this;
    m->renderer = renderer;
    m->fpsAverage = 0.0f;
    m->frameCounter = 0.0f;
    m->closed = false;
    m->frameRate = 16;
    m->fpsLimited = true;
    m->showConsole = false;
}

RootScreen::~RootScreen()
{
    delete m;
}

Renderer *RootScreen::GetRenderer()
{
    return m->renderer;
}

bool RootScreen::HandleEvent(const SDL_Event &event)
{
    return m->HandleEvent(event);
}

void RootScreen::SetCurrentScreen(ScreenPtr &&screen)
{
    m->SetCurrentScreen(move(screen));
}

void RootScreen::PushScreen(ScreenPtr &&screen)
{
    m->PushScreen(move(screen));
}

ScreenPtr RootScreen::PopScreen()
{
    return move(m->PopScreen());
}

void RootScreen::DrawFrame()
{
    m->DrawFrame();
}

int RootScreen::Exec()
{
    return m->Exec();
}
