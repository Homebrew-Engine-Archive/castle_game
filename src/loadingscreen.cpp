#include "loadingscreen.h"

static std::vector<std::string> SetPreloadsList(const std::string &listfile)
{
    std::vector<std::string> list;
    
    std::ifstream fin(listfile);
    std::string buffer;
    while(std::getline(fin, buffer)) {
        if(!buffer.empty())
            list.push_back(buffer);
    }

    return list;
}

int RunLoadingScreen(RootScreen *root)
{
    LoadingScreen ls(root);
    return ls.Exec();
}

LoadingScreen::LoadingScreen(RootScreen *root)
    : m_renderer(root->GetRenderer())
    , m_root(root)
    , m_quit(false)
{
    auto filelist = SetPreloadsList("gm/preloads.txt");
    for(const auto &filename : filelist) {
        std::cout << filename << std::endl;
        ScheduleCacheGM1(filename);
    }

    std::vector<FontAtlasInfo> fontInfo = {};

    for(const auto &info : fontInfo) {
        ScheduleCacheFont(info);
    }
}

void LoadingScreen::ScheduleCacheGM1(const std::string &filename)
{
    auto task = [filename, this]() {
        if(!m_renderer->CacheCollection(filename)) {
            std::ostringstream oss;
            oss << "Unable to load file: " << filename;
            throw std::runtime_error(oss.str());
        }
    };
    m_tasks.push_back(task);
}

void LoadingScreen::ScheduleCacheFont(const FontAtlasInfo &info)
{
    
}

int LoadingScreen::Exec()
{
    const Uint32 frameRate = 5;
    const Uint32 frameInterval = 1000 / frameRate;
    Uint32 lastFrame = 0;
    const Uint32 total = m_tasks.size();
    Uint32 completed = 0;
        
    for(const auto &task : m_tasks) {
        if(lastFrame + frameInterval < SDL_GetTicks()) {
            lastFrame = SDL_GetTicks();
            double done = static_cast<double>(completed) / total;
            Draw(done);
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!m_root->HandleEvent(event))
                return -1;
        }

        task();
        completed += 1;
        
    }
    
    return 0;
}

void LoadingScreen::Draw(double done)
{
    Surface background = m_renderer->QuerySurface("gfx/frontend_loading.tgx");
    Surface frame = m_renderer->BeginFrame();

    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(background);

    SDL_Rect bgAligned = AlignRect(bgRect, frameRect, 0, 0);
    SDL_BlitSurface(background, NULL, frame, &bgAligned);
    
    SDL_Rect barOuter = MakeRect(300, 25);
    SDL_Rect barOuterAligned = AlignRect(barOuter, bgAligned, 0, 0.8f);
    FillFrame(frame, &barOuterAligned, 0x7f000000);
    DrawFrame(frame, &barOuterAligned, 0xff000000);

    SDL_Rect barOuterPadded = PadIn(barOuterAligned, 5);
    SDL_Rect barInner = MakeRect(barOuterPadded.w * done, barOuterPadded.h);
    SDL_Rect barInnerAligned = AlignRect(barInner, barOuterPadded, -1.0f, 0);
    FillFrame(frame, &barInnerAligned, 0xff000000);
    
    m_renderer->EndFrame();
}
