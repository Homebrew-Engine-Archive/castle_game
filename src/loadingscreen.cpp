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

int RunLoadingScreen(RootScreen &root, Renderer &renderer)
{
    LoadingScreen ls(root, renderer);
    return ls.Exec();
}

LoadingScreen::LoadingScreen(RootScreen &root, Renderer &renderer)
    : m_renderer(renderer)
    , m_root(root)
    , m_quit(false)
    , m_files(SetPreloadsList("gm/preloads.txt"))
{
}

int LoadingScreen::Exec()
{
    const Uint32 frameRate = 5;
    const Uint32 frameInterval = 1000 / frameRate;
    Uint32 lastFrame = 0;
    const Uint32 total = m_files.size();
    Uint32 completed = 0;
    
    for(const std::string &file : m_files) {
        if(lastFrame + frameInterval < SDL_GetTicks()) {
            lastFrame = SDL_GetTicks();
            double done = static_cast<double>(completed) / total;
            Draw(done);
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!m_root.HandleEvent(event))
                return -1;
        }

        completed += 1;
        m_renderer.LoadImageCollection(file);
    }
    
    return 0;
}

void LoadingScreen::Draw(double done)
{
    Surface background = m_renderer.QuerySurface("gfx/frontend_loading.tgx");
    Surface frame = m_renderer.BeginFrame();
    
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
    
    m_renderer.EndFrame();
}
