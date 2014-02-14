#include "SDLRenderer.h"

SDLRenderer::SDLRenderer(SDLWindow &window)
{
    rndr = SDL_CreateRenderer(
        window.GetWindow(), -1, SDL_RENDERER_ACCELERATED);
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
    
    SDL_SetColorKey(frameBuffer, SDL_TRUE, 0);
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
    std::shared_ptr<SDLSurface> surface,
    const std::vector<SDL_Rect> &rects)
{
    auto result = resources.find(name);
    
    if(result != resources.end()) {
        SDL_Log("Drawing plain %s already registered.",
                name.c_str());
    }

    auto iterAndResult = resources.insert(
        std::make_pair(
            name,
            DrawingPlain()));

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(rndr, surface->GetSurface());

    if(iterAndResult.second) {
        DrawingPlain &plain = iterAndResult.first->second;
        //plain.surface = surface;
        plain.texture = texture;
        plain.rects = rects;
    }
}

void SDLRenderer::CopyDrawingPlain(
    const std::string &name,
    const SDL_Rect *srcrect,
    SDL_Rect *dstrect)
{
    if(!bfBufferInProgress) {
        SDL_Log("No active buffer available.");
        return;
    }
    
    auto result = resources.find(name);

    if(result == resources.end()) {
        SDL_Log("No such drawing plain %s registered.",
            name.c_str());
        return;
    }

    const DrawingPlain &plain = result->second;

    SDL_Texture *texture = plain.texture;
    SDL_RenderCopy(rndr, texture, srcrect, dstrect);    
    return;
    
    if(!plain.surface) {
        SDL_Log("Drawing plain %s is invalid.", name.c_str());
        return;
    }
    
    SDL_Surface *surface = plain.surface->GetSurface();
    if(SDL_BlitSurface(surface, srcrect, frameBuffer, dstrect)) {
        SDL_Log("Blit surface %s failed: %s",
                name.c_str(), SDL_GetError());
        return;
    }
}

SDL_Rect SDLRenderer::QuerySurfaceRect(
    const std::string &name,
    size_t index)
{
    auto result = resources.find(name);
    if(result == resources.end()) {
        SDL_Log("No such rect found in %s.",
                name.c_str());
        return SDL_Rect();
    }


    const DrawingPlain &plain = result->second;
    if(plain.rects.size() <= index) {
        SDL_Log("No rect with such index %d.",
                index);
        return SDL_Rect();
    }
    
    return plain.rects[index];
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
    if(!bfBufferInProgress) {
        SDL_Log("No active buffers to flush!");
        return;
    }

    return;
    
    SDL_Texture *tmp =
        SDL_CreateTextureFromSurface(rndr, frameBuffer);
    
    if(tmp != NULL) {
        if(SDL_RenderCopy(rndr, tmp, NULL, NULL)) {
            SDL_Log("Failure coping texture: %s",
                    SDL_GetError());
        }
        
        SDL_DestroyTexture(tmp);
    } else {
        SDL_Log("Can't create texture: %s",
                SDL_GetError());
    }
    
    bfBufferInProgress = false;
}
