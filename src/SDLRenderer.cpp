#include "SDLRenderer.h"

SDLRenderer::SDLRenderer(SDLWindow &window)
{
    m_R = SDL_CreateRenderer(
        window.GetWindow(), -1, SDL_RENDERER_ACCELERATED);
    if(m_R == NULL)
        throw SDLError(SDL_GetError());
}

SDLRenderer::~SDLRenderer()
{
    SDL_DestroyRenderer(m_R);
}

void SDLRenderer::Present()
{
    SDL_RenderPresent(m_R);
}

SDL_Renderer* SDLRenderer::GetRenderer()
{
    return m_R;
}

SDL_Texture* SDLRenderer::CreateTextureFromSurface(SDLSurface &surface)
{
    return SDL_CreateTextureFromSurface(m_R, surface.GetSurface());
}
