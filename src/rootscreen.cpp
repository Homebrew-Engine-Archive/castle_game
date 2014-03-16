#include "rootscreen.h"
#include "menu_combat.h"
#include "menu_main.h"
#include "loadingscreen.h"
#include "macrosota.h"
#include "network.h"
#include <sstream>
#include <string>
#include <boost/asio/io_service.hpp>

struct RootScreenPimpl
{
    Renderer *renderer;
    double fpsAverage;
    std::uint64_t frameCounter;
    bool closed;
    int frameRate;
    bool frameLimit;
    std::unique_ptr<Screen> currentScreen;

    bool HandleWindowEvent(const SDL_WindowEvent &window);
    bool HandleKeyboardEvent(const SDL_KeyboardEvent &keyboard);

    RootScreenPimpl(Renderer *renderer);
};

RootScreenPimpl::RootScreenPimpl(Renderer *renderer)
    : renderer(renderer)
    , fpsAverage(0.0f)
    , frameCounter(0)
    , closed(false)
    , frameRate(16)
    , frameLimit(true)
{ }

RootScreen::RootScreen(Renderer *renderer)
    : m(new RootScreenPimpl(renderer))
{ }

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
    switch(event.type) {
    case SDL_WINDOWEVENT:
        return m->HandleWindowEvent(event.window);
    case SDL_QUIT:
        m->closed = true;
        return false;
    case SDL_KEYDOWN:
        return m->HandleKeyboardEvent(event.key);        
    default:
        return true;
    }
}

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
    default:
        return true;
    }
}

void RootScreen::SetCurrentScreen(std::unique_ptr<Screen> &&screen)
{
    m->currentScreen = std::move(screen);
}

void RootScreen::DrawFrame()
{
    Surface frame = m->renderer->BeginFrame();
    m->currentScreen->Draw(frame);

    font_size_t size = 24;
    std::string fontname = "font_stronghold_aa";
    SDL_Color color = MakeColor(255, 255, 255, 255);
    m->renderer->SetColor(color);
    m->renderer->SetFont(fontname, size);

    SDL_Point pos = ShiftPoint(TopLeft(m->renderer->GetOutputSize()), 5, 5);
    
    std::ostringstream oss;
    oss << "FPS: " << m->fpsAverage;
    m->renderer->RenderTextLine(oss.str(), pos);
    
    m->renderer->EndFrame();
}

int RootScreen::Exec()
{
    if(int code = RunLoadingScreen(this)) {
        std::clog << "Loading has been interrupted."
                  << std::endl;
        return code;
    }

    m->currentScreen.reset(new MenuMain(this));

    const std::int64_t msPerSec = 1000;
    const int pollRate = 66;
    const int pollInterval = msPerSec / pollRate;
    
    std::int64_t lastFrame = 0;
    std::int64_t lastPoll = 0;
    std::int64_t lastSecond = 0;
    std::int64_t fpsCounterLastSecond = 0;
    
    boost::asio::io_service io;

    const short port = 4500;
    Server server(io, port);
    server.StartAccept();
    
    while(!m->closed) {

        const std::int64_t frameInterval = msPerSec / m->frameRate;

        const std::int64_t pollStart = SDL_GetTicks();
        if(lastPoll + pollInterval < pollStart) {
            io.poll();
            lastPoll = pollStart;
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!m->currentScreen->HandleEvent(event))
                HandleEvent(event);
        }

        const std::int64_t frameStart = SDL_GetTicks();
        if(lastSecond + msPerSec < frameStart) {
            double fps = m->frameCounter - fpsCounterLastSecond;
            double elapsed = frameStart - lastSecond;
            m->fpsAverage = fps * (msPerSec / elapsed);
            lastSecond = frameStart;
            fpsCounterLastSecond = m->frameCounter;
        }

        if(lastFrame + frameInterval < frameStart) {
            DrawFrame();
            lastFrame = frameStart;
            ++m->frameCounter;
        }

        const std::int64_t delayStart = SDL_GetTicks();
        if(m->frameLimit) {
            const std::int64_t nextTick =
                std::min(lastPoll + pollInterval,
                         lastFrame + frameInterval);
            if(delayStart < nextTick) {
                SDL_Delay(nextTick - delayStart);
            }
        }
    }
    
    return 0;
}
