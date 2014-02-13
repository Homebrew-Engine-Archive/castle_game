#include "SDLRenderer.h"

SDLRenderer::SDLRenderer(SDLWindow &window)
{
    rndr = SDL_CreateRenderer(
        window.GetWindow(), -1, 0);
    if(rndr == NULL)
        throw SDLError(SDL_GetError());

    bfBufferInProgress = false;
    InitialSetSize();
}

SDLRenderer::~SDLRenderer()
{
    SDL_DestroyRenderer(rndr);
    SDL_FreeSurface(frameBuffer);
}

void SDLRenderer::InitialSetSize()
{
    int width;
    int height;
    SDL_GetRendererOutputSize(rndr, &width, &height);

    frameBuffer = NULL;
    ResizeOutput(width, height);
}

void SDLRenderer::ResizeOutput(int width, int height)
{
    if(frameBuffer != NULL)
        SDL_FreeSurface(frameBuffer);

    frameBuffer =
        SDL_CreateRGBSurface(
            0, width, height, 16,
            TGX_RGB16_RMASK,
            TGX_RGB16_GMASK,
            TGX_RGB16_BMASK,
            TGX_RGB16_AMASK);
}

void SDLRenderer::Present()
{
    SDL_RenderPresent(rndr);
}

SDL_Renderer* SDLRenderer::GetRenderer()
{
    return rndr;
}

void SDLRenderer::RegisterDrawingPlain(
    const std::string &name,
    std::shared_ptr<SDLSurface> surface)
{
    auto result = surfaces.find(name);
    
    if(result != surfaces.end()) {
        SDL_Log("Drawing plain %s already registered.",
                name.c_str());
    }
    
    surfaces.insert(
        std::make_pair(
            name,
            surface));
}

void SDLRenderer::CopyDrawingPlain(
    const std::string &name,
    const SDL_Rect *srcrect,
    SDL_Rect *dstrect)
{
    if(frameBuffer == NULL || !bfBufferInProgress) {
        SDL_Log("No active buffer available.");
        return;
    }
    
    auto result = surfaces.find(name);

    if(result == surfaces.end()) {
        SDL_Log("No such drawing plain %s registered.",
            name.c_str());
        return;
    }

    std::shared_ptr<SDLSurface> surface =
        result->second;
    
    if(!surface) {
        SDL_Log("Drawing plain %s is invalid.", name.c_str());
        return;
    }

    SDL_BlitSurface(surface->GetSurface(), srcrect,
                    frameBuffer, dstrect);
}

void SDLRenderer::RegisterSurfaceRect(
    const std::string &name,
    size_t index,
    const SDL_Rect &rect)
{
    auto key = std::make_pair(name, index);
    auto result = layouts.find(key);

    if(result != layouts.end()) {
        SDL_Log("Rect is already registered in (%s, %d).",
                name.c_str(), index);
        return;
    }
    
    layouts.insert(
        std::make_pair(key, rect));
}

SDL_Rect SDLRenderer::QuerySurfaceRect(
    const std::string &name,
    size_t index)
{
    auto key = std::make_pair(name, index);
    auto result = layouts.find(key);

    if(result == layouts.end()) {
        SDL_Log("No such rect found in (%s, %d).",
                name.c_str(), index);
        return SDL_Rect();
    }

    return result->second;
}

void SDLRenderer::BeginFrame()
{
    if(SDL_RenderSetClipRect(rndr, NULL)) {
        SDL_Log("Can't reset clip rect.");
    }

    if(SDL_RenderSetViewport(rndr, NULL)) {
        SDL_Log("Can't reset viewport.");
    }

    bfBufferInProgress = true;
}

void SDLRenderer::EndFrame()
{
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(rndr, frameBuffer);

    if(texture != NULL) {
        SDL_RenderCopy(rndr, texture, NULL, NULL);
        SDL_DestroyTexture(texture);
    } else {
        SDL_Log("Can't create texture: %s",
                SDL_GetError());
    }
    
    bfBufferInProgress = false;
}
