#include "main.h"

#include <vector>
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

template<class T>
void PrettyPrintVector(std::ostream &out, const std::vector<T> &xs)
{
    if(xs.size() != 0) {
        out << xs[0];
        for(size_t i = 1; i < xs.size(); ++i) {
            out << ", " << xs[i];
        }
    } else {
        out << "<None>";
    }
}

void PrintRendererInfo(std::ostream &out, const SDL_RendererInfo &info)
{
    using namespace std;
    
    out << "\tName: "
        << info.name
        << endl;
            
    out << "\tNumber texture formats: "
        << dec
        << info.num_texture_formats
        << endl;
            
    out << "\tMax texture width: "
        << dec
        << info.max_texture_width
        << endl;
            
    out << "\tMax texture height: "
        << dec
        << info.max_texture_height
        << endl;

    std::vector<int> textureFormats;
    for(size_t i = 0; i < info.num_texture_formats; ++i)
        textureFormats.push_back(info.texture_formats[i]);
    
    out << "\tTexture formats: ";
    PrettyPrintVector(out, textureFormats);
    out << dec << endl;

    std::vector<std::string> flagNames;
    if(info.flags & SDL_RENDERER_SOFTWARE)
        flagNames.push_back("Software");
    if(info.flags & SDL_RENDERER_ACCELERATED)
        flagNames.push_back("Accelerated");
    if(info.flags & SDL_RENDERER_PRESENTVSYNC)
        flagNames.push_back("PresentVSync");
    if(info.flags & SDL_RENDERER_TARGETTEXTURE)
        flagNames.push_back("TargetTexture");
    
    out << "\tflags: ";
    PrettyPrintVector(out, flagNames);
    out << endl;
}
