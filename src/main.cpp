#include "main.h"
#include <iostream>
#include "SDL.h"
#include "sdl_utils.h"
#include "rootscreen.h"
#include "sdl_init.h"
#include "renderer.h"

using namespace std;

void EnumRenderDrivers(ostream &out)
{
    int num = SDL_GetNumRenderDrivers();
    out << "Drivers avialable: "
        << dec << num
        << endl;

    for(int index = 0; index < num; ++index) {        
        out << "Driver with index: "
            << dec << index
            << endl;
        
        SDL_RendererInfo info;
        if(SDL_GetRenderDriverInfo(index, &info)) {
            out << "Can't query driver info"
                << endl;
        } else {
            PrintRendererInfo(out, info);
        }        
    }
}

void GetAndPrintRendererInfo(ostream &out, SDL_Renderer *renderer)
{
    if(renderer != NULL) {
        out << "Renderer info: " << endl;
        
        SDL_RendererInfo info;
        if(SDL_GetRendererInfo(renderer, &info)) {
            out << "\tCan't query renderer info" << endl;
        } else {
            PrintRendererInfo(out, info);
        }
    }
}

int main()
{
    SDLInit(SDL_INIT_EVERYTHING);

    EnumRenderDrivers(clog);

    const int screenwidth = 1024;
    const int screenheight = 768;
        
    WindowPtr sdl_window = WindowPtr(
        SDL_CreateWindow(
            "Stockade",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screenwidth, screenheight,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
    ThrowSDLError(sdl_window);

    RendererPtr sdl_renderer = RendererPtr(
        SDL_CreateRenderer(sdl_window.get(), -1, 0));
    ThrowSDLError(sdl_renderer);

    GetAndPrintRendererInfo(clog, sdl_renderer.get());
    
    unique_ptr<Renderer> &&renderer = CreateRenderer(sdl_renderer.get());
    
    unique_ptr<RootScreen> &&root = CreateRootScreen(renderer.get());

    return root->Exec();
}
