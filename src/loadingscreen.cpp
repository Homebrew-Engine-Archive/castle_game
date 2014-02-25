#include "loadingscreen.h"

static std::vector<std::string> SetPreloadsList(const std::string &listfile)
{
    std::vector<std::string> list;
    
    std::ifstream fin(listfile);
    std::string buffer;
    while(std::getline(fin, buffer)) {
        if(buffer.empty())
            continue;
        list.push_back(buffer);
    }

    return list;
}

static int HandleEvents()
{
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
        case SDL_QUIT:
            return -1;
        case SDL_KEYDOWN:
            if(e.key.keysym.sym == SDLK_ESCAPE)
                return -1;
        default:
            break;
        }
    }
    
    return 0;
}

static void DrawLoadingScreen(Renderer &renderer, double done)
{
    renderer.Clear();
    renderer.BlitImage("gfx/frontend_loading.tgx", NULL, NULL);
        
    SDL_Rect bar;
    bar.x = 0;
    bar.y = 0;
    bar.w = 1024 * done;
    bar.h = 10;
    renderer.FillRect(&bar);
    renderer.Present();
}

int RunLoadingScreen(Renderer &renderer)
{
    std::ifstream fin("gm/preloads.txt");
    std::vector<std::string> files =
        SetPreloadsList("gm/preloads.txt");

    renderer.LoadImage("gfx/frontend_loading.tgx");

    size_t total = files.size();
    size_t completed = 0;

    Uint32 lastDrawn = 0;
    const Uint32 frameRate = 8;
    const Uint32 frameDelay = 1000 / frameRate;
    
    for(const auto &filename : files) {

        const Uint32 currentTime = SDL_GetTicks();

        if(lastDrawn + frameDelay < currentTime) {
            lastDrawn = currentTime;
            DrawLoadingScreen(
                renderer,
                static_cast<double>(completed) / total);
        }

        renderer.LoadImageCollection(filename);
        if(HandleEvents())
            return -1;
        
        ++completed;        
    }
    
    return 0;
}
