#include "rootscreen.h"

RootScreen::RootScreen(Renderer *renderer)
    : m_renderer(renderer)
    , m_fpsAverage(0.0f)
    , m_closed(false)
    , m_frameRate(16)
    , m_frameLimit(true)
{
}

bool RootScreen::HandleEvent(const SDL_Event &event)
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

bool RootScreen::HandleWindowEvent(const SDL_WindowEvent &window)
{
    switch(window.event) {
    case SDL_WINDOWEVENT_RESIZED:
        {
            int width = window.data1;
            int height = window.data2;
            m_renderer->AdjustOutputSize(width, height);
        }
        return true;

    default:
        return true;
    }
}

bool RootScreen::HandleKeyboardEvent(const SDL_KeyboardEvent &key)
{
    switch(key.keysym.sym) {
    case SDLK_ESCAPE:
        m_closed = true;
        return false;
        
    default:
        return true;
    }
}

void RootScreen::SetCurrentScreen(std::unique_ptr<Screen> screen)
{
    m_currentScreen = std::move(screen);
}

void RootScreen::DrawFrame()
{
    Surface frame = m_renderer->BeginFrame();
    m_currentScreen->Draw(frame);

    std::ostringstream oss;
    oss << "FPS: " << m_fpsAverage;

    m_renderer->RenderTextLine(oss.str(), NULL, std::string("stronghold_aa"), MakeColor(255, 255, 255, 128), 16);
        
    m_renderer->EndFrame();    
}

int RootScreen::Exec()
{
    if(int code = RunLoadingScreen(this)) {
        std::clog << "Loading has been interrupted."
                  << std::endl;
        return code;
    }

    //m_currentScreen.reset(new MenuMain(this));
    m_currentScreen.reset(new GameScreen(this));

    const std::int64_t msPerSec = 1000;
    const int pollRate = 66;
    const int pollInterval = msPerSec / pollRate;
    
    std::int64_t lastFrame = 0;
    std::int64_t lastPoll = 0;
    std::int64_t lastSecond = 0;
    std::int64_t fpsCounterLastSecond = 0;
    std::int64_t fpsCounter = 0;
    
    boost::asio::io_service io;

    const short port = 4500;
    Server server(io, port);
    server.StartAccept();
    
    while(!m_closed) {
        const std::int64_t frameInterval = msPerSec / m_frameRate;

        const std::int64_t pollStart = SDL_GetTicks();
        if(lastPoll + pollInterval < pollStart) {
            io.poll();
            lastPoll = pollStart;
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!m_currentScreen->HandleEvent(event))
                HandleEvent(event);
        }

        const std::int64_t frameStart = SDL_GetTicks();
        if(lastSecond + msPerSec < frameStart) {
            double fps = fpsCounter - fpsCounterLastSecond;
            double elapsed = frameStart - lastSecond;
            m_fpsAverage = fps * (msPerSec / elapsed);
            lastSecond = frameStart;
            fpsCounterLastSecond = fpsCounter;
        }

        DrawFrame();
        lastFrame = frameStart;
        ++fpsCounter;

        const std::int64_t delayStart = SDL_GetTicks();
        if(m_frameLimit) {
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
