#include "renderer.h"

Renderer::Renderer(SDLWindow &window)
{
    rndr = SDL_CreateRenderer(
        window.GetWindow(), -1,
        SDL_RENDERER_ACCELERATED);

    if(rndr == NULL)
        throw SDLError(SDL_GetError());
}

Renderer::~Renderer()
{
    SDL_DestroyRenderer(rndr);
}

void Renderer::Present()
{
    SDL_RenderPresent(rndr);
}

void Renderer::BeginFrame()
{
    if(SDL_RenderSetClipRect(rndr, NULL)) {
        SDL_Log("Can't reset clip rect.");
    }

    if(SDL_RenderSetViewport(rndr, NULL)) {
        SDL_Log("Can't reset viewport.");
    }
}

void Renderer::EndFrame()
{
}

void Renderer::Clear()
{
    SDL_RenderClear(rndr);
}

void Renderer::FillRect(const SDL_Rect *rect)
{
    SDL_RenderFillRect(rndr, rect);
}

void Renderer::BlitImage(const std::string &name, const SDL_Rect *srcrect, const SDL_Rect *dstrect)
{
    auto searchResult = imageStorage.find(name);

    if(searchResult != imageStorage.end()) {
        Surface surface = searchResult->second;
        
        SDL_Texture *texture =
            SDL_CreateTextureFromSurface(rndr, surface);
        
        if(texture == NULL) {
            SDL_LogDebug(SDL_LOG_PRIORITY_ERROR, "Unable to create texture: %s",
                         SDL_GetError());
            return;
        }

        if(SDL_RenderCopy(rndr, texture, srcrect, dstrect)) {
            SDL_LogDebug(SDL_LOG_PRIORITY_ERROR, "Unable to blit texture: %s",
                         SDL_GetError());
        }

        SDL_DestroyTexture(texture);        
    }
}

int Renderer::LoadImage(const std::string &filename)
{
    SDL_Log("LoadImage: %s", filename.c_str());
    try {
        std::unique_ptr<SDL_RWops, RWCloseDeleter> src(
            SDL_RWFromFile(filename.c_str(), "rb"));

        if(!src)
            throw IOError("file not readable");
        
        Surface image = tgx::LoadTGX(src.get());
        
        imageStorage.insert(
            std::make_pair(filename, image));
        
    } catch(const std::exception &e) {
        SDL_Log("LoadImage %s error: %s",
                filename.c_str(), e.what());
        return -1;
    }
    
    return 0;
}

int Renderer::LoadImageCollection(const std::string &filename)
{
    SDL_Log("LoadImageCollection: %s", filename.c_str());
    try {
        std::unique_ptr<SDL_RWops, RWCloseDeleter> src(
            SDL_RWFromFile(filename.c_str(), "rb"));

        if(!src)
            throw IOError("file not readable");
        
        gm1::Collection gm1(src.get());
        Surface atlas = gm1::LoadAtlas(src.get(), gm1);
        atlasStorage.insert(
            std::make_pair(filename, atlas));
    } catch(const std::exception &e) {
        SDL_Log("LoadImageCollection %s error: %s",
                filename.c_str(), e.what());
        return -1;
    }

    return 0;
}
