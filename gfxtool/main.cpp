#include "main.h"

#include <sstream>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <SDL.h>

#include <game/sdl_utils.h>
#include <game/sdl_init.h>
#include <game/surface.h>
#include <game/tgx.h>

int main(int argc, char *argv[])
{
    if(argc < 2) {
        std::cout << "Use string, luke!" << std::endl;
    }

    std::string name = argv[1];

    std::ifstream fin(name, std::ios_base::binary);
    if(!fin.is_open()) {
        throw std::runtime_error("Can't open file");
    }
    
    Surface surf = NULL;
    TGX::ReadSurfaceHeader(fin, surf);
    SDL_FillRect(surf, NULL, 0x8000);

    std::streampos origin = fin.tellg();
    fin.seekg(0, std::ios_base::end);
    std::streampos fsize = fin.tellg();
    fin.seekg(origin);
    
    TGX::DecodeSurface(fin, fsize, surf);
    
    return ShowSurface(surf);
}

int ShowSurface(const Surface &surface)
{
    Init::SDLInit initializer();

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    SDL_CreateWindowAndRenderer(surface->w, surface->h, 0, &window, &renderer);
    
    if(!window || !renderer) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    bool quit = false;
    while(!quit) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                quit = true;
        }
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    return 0;
}
