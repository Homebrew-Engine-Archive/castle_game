#include "main.h"

#include <iostream>

#include "sdl_utils.h"
#include "engine.h"
#include "sdl_init.h"
#include "renderer.h"

int main()
{
    Init::SDLInit init();
    
    EnumRenderDrivers(std::clog);

    const int screenwidth = 1024;
    const int screenheight = 768;
        
    WindowPtr sdlWindow = WindowPtr(
        SDL_CreateWindow(
            "Stockade",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screenwidth, screenheight,
            SDL_WINDOW_OPENGL));
    if(!sdlWindow) {
        throw std::runtime_error(SDL_GetError());
    }

    RendererPtr sdlRenderer = RendererPtr(
        SDL_CreateRenderer(sdlWindow.get(), -1, 0));
    if(!sdlRenderer) {
        throw std::runtime_error(SDL_GetError());
    }

    GetAndPrintRendererInfo(std::clog, sdlRenderer.get());
    
    std::unique_ptr<Render::Renderer> &&renderer = Render::CreateRenderer(sdlRenderer.get());
    std::unique_ptr<Castle::Engine> root(new Castle::Engine(renderer.get()));
    return root->Exec();
}

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
