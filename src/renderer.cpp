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

void Renderer::LoadGM1Atlas(SDL_RWops *src, const gm1::Collection &gm1)
{    
    try {
        std::shared_ptr<Surface> atlas = gm1::LoadAtlas(src, gm1);

        atlases.push_back(atlas);
        
        gm1::Encoding encoding = gm1::GetEncoding(gm1.header);
        switch(encoding) {
        case gm1::Encoding::TGX8:
            break;
        case gm1::Encoding::TGX16:
            break;
        case gm1::Encoding::Bitmap:
            break;
        case gm1::Encoding::TileObject:
            break;
        default:
            throw gm1::GM1Error("Unknown encoding");
        }
    } catch(const std::exception &e) {
        SDL_Log("Exception handled in LoadGM1Atlas:");
        SDL_Log("\t%s", e.what());
    }
}
