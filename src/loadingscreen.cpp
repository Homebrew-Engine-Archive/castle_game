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

void LoadingScreen::HandleEvents()
{
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
        case SDL_QUIT:
            m_quit = true;
            break;
        case SDL_KEYDOWN:
            if(e.key.keysym.sym == SDLK_ESCAPE)
                m_quit = true;
            break;
        default:
            break;
        }
    }
}

LoadingScreen::LoadingScreen(Renderer &renderer)
    : m_renderer(renderer)
    , m_background(renderer.LoadImage("gfx/frontend_loading.tgx"))
    , m_quit(false)
    , m_files(SetPreloadsList("gm/preloads.txt"))
{
}

int LoadingScreen::Exec()
{
    size_t total = m_files.size();
    size_t completed = 0;

    Uint32 lastDrawn = 0;
    const Uint32 frameRate = 5;
    const Uint32 frameDelay = 1000 / frameRate;
    
    for(const auto &filename : m_files) {
        HandleEvents();
        if(m_quit)
            return -1;
        const Uint32 currentTime = SDL_GetTicks();
        if(lastDrawn + frameDelay < currentTime) {
            lastDrawn = currentTime;
            Draw(static_cast<double>(completed) / total);
        }
        m_renderer.LoadImageCollection(filename);
        ++completed;
    }
    
    return 0;
}

void LoadingScreen::Draw(double done)
{
    Surface frame = m_renderer.BeginFrame();

    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(m_background);
    SDL_Rect bgAligned = AlignRect(bgRect, frameRect, 0, 0);
    BlitSurface(m_background, NULL, frame, &bgAligned);
    
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

int RunLoadingScreen(Renderer &renderer)
{
   LoadingScreen ls(renderer);
   return ls.Exec();
}
