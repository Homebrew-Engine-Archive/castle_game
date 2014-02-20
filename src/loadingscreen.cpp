#include "loadingscreen.h"

LoadingScreen::LoadingScreen(Renderer &renderer)
    : renderer(renderer)
    , m_closed(false)
    , m_completed(0)
{
    renderer.LoadImage("gfx/frontend_loading.tgx");
    SetPreloadsList("gm/preloads.txt");
    m_total = m_files.size();
}

LoadingScreen::~LoadingScreen()
{
}

void LoadingScreen::OnEnterEventLoop()
{
    auto nextfile = m_files.begin();

    if(nextfile != m_files.end()) {
        renderer.LoadImageCollection(*nextfile);
        m_files.erase(m_files.begin());
        ++m_completed;
        if(m_files.empty()) {
             m_nextscr.reset(new GameScreen);
             m_closed = true;
        }
    }
}

void LoadingScreen::OnFrame(Renderer &renderer)
{
    renderer.Clear();
    renderer.BlitImage("gfx/frontend_loading.tgx", NULL, NULL);

    SDL_Rect bar;
    bar.x = 0;
    bar.y = 0;
    bar.w = 1024 * GetPercentageComplete();
    bar.h = 10;
    renderer.FillRect(&bar);
}

double LoadingScreen::GetPercentageComplete() const
{
    if(m_total == 0)
        return 1.0f;
    
    return static_cast<double>(m_completed) /
        static_cast<double>(m_total);
}

void LoadingScreen::OnEvent(const SDL_Event &)
{
}

std::unique_ptr<Screen> LoadingScreen::NextScreen()
{
    return std::move(m_nextscr);
}

bool LoadingScreen::Closed() const
{
    return m_closed;
}

void LoadingScreen::SetPreloadsList(const std::string &fn)
{
    std::ifstream fin(fn);
    std::string buffer;
    while(std::getline(fin, buffer)) {
        if(buffer.empty())
            continue;
        m_files.insert(m_files.end(), buffer);
    }
}
