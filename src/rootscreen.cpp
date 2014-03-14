#include "rootscreen.h"

RootScreen::RootScreen(Renderer *renderer)
    : m_renderer(renderer)
    , m_fpsAverage(0.0f)
    , m_frameCounter(0)      
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
    LogWindowEvent(window);
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

void RootScreen::LogWindowEvent(const SDL_WindowEvent &window)
{
    switch (window.event) {
    case SDL_WINDOWEVENT_SHOWN:
        SDL_Log("Window %d shown", window.windowID);
        break;
    case SDL_WINDOWEVENT_HIDDEN:
        SDL_Log("Window %d hidden", window.windowID);
        break;
    case SDL_WINDOWEVENT_EXPOSED:
        SDL_Log("Window %d exposed", window.windowID);
        break;
    case SDL_WINDOWEVENT_MOVED:
        SDL_Log("Window %d moved to %d,%d",
                window.windowID, window.data1,
                window.data2);
        break;
    case SDL_WINDOWEVENT_RESIZED:
        SDL_Log("Window %d resized to %dx%d",
                window.windowID, window.data1,
                window.data2);
        break;
    case SDL_WINDOWEVENT_MINIMIZED:
        SDL_Log("Window %d minimized", window.windowID);
        break;
    case SDL_WINDOWEVENT_MAXIMIZED:
        SDL_Log("Window %d maximized", window.windowID);
        break;
    case SDL_WINDOWEVENT_RESTORED:
        SDL_Log("Window %d restored", window.windowID);
        break;
    case SDL_WINDOWEVENT_ENTER:
        SDL_Log("Mouse entered window %d",
                window.windowID);
        break;
    case SDL_WINDOWEVENT_LEAVE:
        SDL_Log("Mouse left window %d", window.windowID);
        break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        SDL_Log("Window %d gained keyboard focus",
                window.windowID);
        break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        SDL_Log("Window %d lost keyboard focus",
                window.windowID);
        break;
    case SDL_WINDOWEVENT_CLOSE:
        SDL_Log("Window %d closed", window.windowID);
        break;
    default:
        SDL_Log("Window %d got unknown event %d",
                window.windowID, window.event);
        break;
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

    font_size_t size = 24;
    std::string fontname = "stronghold_aa";
    SDL_Color color = MakeColor(255, 255, 255, 255);
    m_renderer->SetColor(color);
    m_renderer->SetFont(fontname, size);

    SDL_Point pos = ShiftPoint(TopLeft(m_renderer->GetOutputSize()), 5, 5);
    
    std::ostringstream oss;
    oss << "FPS: " << m_fpsAverage;
    m_renderer->RenderTextLine(oss.str(), pos);
    
    m_renderer->EndFrame();
}

int RootScreen::Exec()
{
    if(int code = RunLoadingScreen(this)) {
        std::clog << "Loading has been interrupted."
                  << std::endl;
        return code;
    }

    m_currentScreen.reset(new MenuMain(this));

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
