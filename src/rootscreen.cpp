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

class Implementation : public RootScreen
{
    Renderer *m_renderer;
    double m_fpsAverage;
    uint64_t m_frameCounter;
    bool m_closed;
    int m_frameRate;
    bool m_fpsLimited;
    bool m_showConsole;
    ScreenPtr m_debugConsole;
    vector<ScreenPtr> m_screenStack;
        
    bool HandleWindowEvent(const SDL_WindowEvent &event);
    bool HandleKeyboardEvent(const SDL_KeyboardEvent &event);
    Screen *GetCurrentScreen() const;
    void ToggleConsole();
    
public:
    Implementation(Renderer *m_renderer);
    
    int Exec();
    void DrawFrame();    
    bool HandleEvent(const SDL_Event &event);
    void SetCurrentScreen(ScreenPtr &&screen);
    void PushScreen(ScreenPtr &&screen);
    ScreenPtr PopScreen();
    Renderer *GetRenderer();

};

Implementation::Implementation(Renderer *renderer)
    : m_renderer(renderer)
    , m_fpsAverage(0.0f)
    , m_frameCounter(0.0f)
    , m_closed(false)
    , m_frameRate(16)
    , m_fpsLimited(true)
    , m_showConsole(false)
    , m_debugConsole(new DebugConsole(this))
{
}

bool Implementation::HandleWindowEvent(const SDL_WindowEvent &window)
{
    switch(window.event) {
    case SDL_WINDOWEVENT_RESIZED:
        {
            int width = window.data1;
            int height = window.data2;
            m_renderer->AdjustBufferSize(width, height);
        }
        return true;
    default:
        return true;
    }
}

bool Implementation::HandleKeyboardEvent(const SDL_KeyboardEvent &key)
{
    switch(key.keysym.sym) {
    case SDLK_ESCAPE:
        m_closed = true;
        return false;
    case SDLK_BACKSLASH:
        ToggleConsole();
        return true;
    default:
        return true;
    }
}

void Implementation::ToggleConsole()
{
    m_showConsole = !m_showConsole;

    if(m_showConsole) {
        PushScreen(move(m_debugConsole));
    } else {
        m_debugConsole = move(PopScreen());
    }
}

Screen *Implementation::GetCurrentScreen() const
{
    if(m_screenStack.empty())
        return NULL;
    else
        return m_screenStack.back().get();
}

void Implementation::SetCurrentScreen(ScreenPtr &&screen)
{
    m_screenStack.clear();
    PushScreen(move(screen));
}

void Implementation::PushScreen(ScreenPtr &&screen)
{
    m_screenStack.push_back(move(screen));
}

ScreenPtr Implementation::PopScreen()
{
    if(!m_screenStack.empty()) {
        ScreenPtr ptr = std::move(m_screenStack.back());
        m_screenStack.pop_back();
        return ptr;
    }

    return ScreenPtr(nullptr);
}

int Implementation::Exec()
{
    if(int code = RunLoadingScreen(this)) {
        clog << "Loading has been interrupted."
                  << endl;
        return code;
    }

    PushScreen(make_unique<MenuMain>(this));

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
    
    while(!m_closed) {
        const int64_t frameInterval = msPerSec / m_frameRate;
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
            double fps = m_frameCounter - fpsCounterLastSecond;
            double elapsed = frameStart - lastSecond;
            m_fpsAverage = fps * (msPerSec / elapsed);
            lastSecond = frameStart;
            fpsCounterLastSecond = m_frameCounter;
        }

        if(lastFrame + frameInterval < frameStart) {
            DrawFrame();
            lastFrame = frameStart;
            ++m_frameCounter;
        }

        const int64_t delayStart = SDL_GetTicks();
        if(m_fpsLimited) {
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

void Implementation::DrawFrame()
{
    Surface frame = m_renderer->BeginFrame();

    for(ScreenPtr &ptr : m_screenStack) {
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
    m_renderer->SetColor(color);
    m_renderer->SetFont(fontname, size);

    SDL_Point pos = ShiftPoint(TopLeft(m_renderer->GetOutputSize()), 5, 5);
    
    ostringstream oss;
    oss << "FPS: " << m_fpsAverage;
    m_renderer->RenderTextLine(oss.str(), pos);

    m_renderer->EndFrame();
}

bool Implementation::HandleEvent(const SDL_Event &event)
{
    switch(event.type) {
    case SDL_WINDOWEVENT:
        return HandleWindowEvent(event.window);
    case SDL_QUIT:
        m_closed = true;
        return false;
    case SDL_KEYDOWN:
        return HandleKeyboardEvent(event.key);
    default:
        return true;
    }
}

Renderer *Implementation::GetRenderer()
{
    return m_renderer;
}

RootScreen *CreateRootScreen(Renderer *renderer)
{
    return new Implementation(renderer);
}
