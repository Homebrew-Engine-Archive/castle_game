#include "main.h"

#include <cstring>
#include <cassert>

#include <chrono>
#include <sstream>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <SDL.h>

#include <game/sdl_error.h>
#include <game/sdl_utils.h>
#include <game/image.h>
#include <game/tgx.h>

int main(int argc, char *argv[])
{
    if(argc < 2) {
        std::cout << "Use string, luke!" << std::endl;
    }
    
    const std::string name = argv[1];

    std::ifstream fin(name, std::ios_base::binary);
    if(!fin.is_open()) {
        std::ostringstream oss;
        oss << "can't open file: " << strerror(errno);
        throw std::runtime_error(oss.str());
    }
    
    const castle::Image image = tgx::ReadImage(fin);
    return gfxtool::ShowImage(image);
}

namespace gfxtool
{
    int ShowImage(const castle::Image &image)
    {
        SDLInitializer init(SDL_INIT_EVERYTHING);

        WindowPtr window(
            SDL_CreateWindow("Gfx viewer",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             image.Width(), image.Height(),
                             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
        if(!window) {
            throw sdl_error();
        }
        
        RendererPtr renderer(
            SDL_CreateRenderer(window.get(), -1, 0));
        if(!renderer) {
            throw sdl_error();
        }

        TexturePtr texture(SDL_CreateTextureFromSurface(renderer.get(), image.GetSurface()));
        if(texture == NULL) {
            throw sdl_error();
        }

        bool quit = false;
        while(!quit) {
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                if(event.type == SDL_QUIT) {
                    quit = true;
                }
            }
            std::chrono::steady_clock::time_point beforeFrame = std::chrono::steady_clock::now();
            if(SDL_RenderCopy(renderer.get(), texture.get(), NULL, NULL) < 0) {
                throw sdl_error();
            }
            SDL_RenderPresent(renderer.get());
            std::chrono::steady_clock::time_point afterFrame = std::chrono::steady_clock::now();
            std::chrono::nanoseconds ms = std::chrono::duration_cast<std::chrono::nanoseconds>(afterFrame - beforeFrame);
            std::cout << "Frame took " << ms.count() << " ns" << std::endl;
        }
    
        return 0;
    }
}
