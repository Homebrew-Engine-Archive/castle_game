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

SDL_Rect Renderer::OutputRect() const
{
    int w, h;
    if(SDL_GetRendererOutputSize(rndr, &w, &h)) {
        SDL_Log("SDL_GetRendererOutputSize failed: %s", SDL_GetError());
        return MakeEmptyRect();
    }

    return MakeRect(w, h);
}

void Renderer::BlitCollectionImage(const std::string &filename, size_t index)
{
    Surface surface = atlasStorage[filename];
    SDL_Palette *palette = atlasPalettes[filename];
    const SDL_Rect &rect = atlasPartition[filename][index];

    Surface dst = CopySurface(surface, &rect);
    
    if(dst->format->BitsPerPixel == 8) {
        SDL_SetSurfacePalette(dst, palette);
    }
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(rndr, dst);
    SDL_RenderCopy(rndr, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
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

bool Renderer::LoadImage(const std::string &filename)
{
    SDL_Log("LoadImage: %s", filename.c_str());
    try {
        std::unique_ptr<SDL_RWops, RWCloseDeleter> src(
            SDL_RWFromFile(filename.c_str(), "rb"));

        if(!src)
            throw IOException("file not readable");
        
        Surface image = tgx::LoadTGX(src.get());
        
        imageStorage.insert(
            std::make_pair(filename, image));
        
    } catch(const std::exception &e) {
        SDL_Log("LoadImage %s error: %s",
                filename.c_str(), e.what());
        return false;
    }

    return true;
}

bool Renderer::LoadImageCollection(const std::string &filename)
{
    SDL_Log("LoadImageCollection: %s", filename.c_str());
    try {
        std::unique_ptr<SDL_RWops, RWCloseDeleter> src(
            SDL_RWFromFile(filename.c_str(), "rb"));

        if(!src)
            throw IOException("file not readable");
        
        gm1::Collection gm1(src.get());
        Surface atlas = gm1::LoadAtlas(src.get(), gm1);
        atlasStorage.insert(
            std::make_pair(filename, atlas));
        atlasPartition[filename] = gm1::EvalAtlasPartition(gm1);
        atlasPalettes[filename] = gm1::CreateSDLPaletteFrom(gm1.palettes[3]);
        
    } catch(const std::exception &e) {
        SDL_Log("LoadImageCollection %s error: %s",
                filename.c_str(), e.what());
        return 0;
    }

    return true;
}
