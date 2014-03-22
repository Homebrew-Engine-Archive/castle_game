#include "main.h"

#include <iostream>

#include <SDL.h>
#include "sdl_utils.h"
#include "engine.h"
#include "sdl_init.h"
#include "renderer.h"

void EnumRenderDrivers(std::ostream &out)
{
    int num = SDL_GetNumRenderDrivers();
    out << "Drivers avialable: "
        << std::dec << num
        << std::endl;

    for(int index = 0; index < num; ++index) {        
        out << "Driver with index: "
            << std::dec << index
            << std::endl;
        
        SDL_RendererInfo info;
        if(SDL_GetRenderDriverInfo(index, &info)) {
            out << "Can't query driver info"
                << std::endl;
        } else {
            PrintRendererInfo(out, info);
        }        
    }
}

void GetAndPrintRendererInfo(std::ostream &out, SDL_Renderer *renderer)
{
    if(renderer != NULL) {
        out << "Renderer info: " << std::endl;
        
        SDL_RendererInfo info;
        if(SDL_GetRendererInfo(renderer, &info)) {
            out << "\tCan't query renderer info" << std::endl;
        } else {
            PrintRendererInfo(out, info);
        }
    }
}

int main()
{
    SDLInit(SDL_INIT_EVERYTHING);

    EnumRenderDrivers(std::clog);

    const int screenwidth = 1024;
    const int screenheight = 768;
        
    WindowPtr sdlWindow = WindowPtr(
        SDL_CreateWindow(
            "Stockade",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screenwidth, screenheight,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
    ThrowSDLError(sdlWindow);

    RendererPtr sdlRenderer = RendererPtr(
        SDL_CreateRenderer(sdlWindow.get(), -1, 0));
    ThrowSDLError(sdlRenderer);

    GetAndPrintRendererInfo(std::clog, sdlRenderer.get());
    
    std::unique_ptr<Renderer> &&renderer = CreateRenderer(sdlRenderer.get());
    
    std::unique_ptr<Castle::Engine> &&root = Castle::CreateEngine(renderer.get());

    return root->Exec();
}
