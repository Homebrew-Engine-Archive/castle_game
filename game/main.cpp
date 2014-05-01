#include "main.h"

#include <SDL.h>

#include <vector>
#include <iostream>

#include <game/gameexception.h>
#include <game/sdl_utils.h>
#include <game/engine.h>
#include <game/renderer.h>

int main()
{
    const int screenwidth = 1024;
    const int screenheight = 768;

    WindowPtr sdlWindow;
    RendererPtr sdlRenderer;

    try {
        SDLInitializer init();
        
        sdlWindow.reset(
            SDL_CreateWindow(
                "Stockade",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                screenwidth, screenheight,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));

        if(!sdlWindow) {
            throw std::runtime_error(SDL_GetError());
        }

        sdlRenderer.reset(
            SDL_CreateRenderer(sdlWindow.get(), -1, 0));
        
        if(!sdlRenderer) {
            throw std::runtime_error(SDL_GetError());
        }

        std::unique_ptr<Render::Renderer> renderer;
        renderer.reset(new Render::Renderer(sdlRenderer.get()));

        std::unique_ptr<Castle::Engine> engine;
        engine.reset(new Castle::Engine(renderer.get()));

        int code = engine->Exec();
        std::clog << "Shutting down" << std::endl;
        return code;
        
    } catch(const GameException &error) {
        std::cerr << error << std::endl;
    } catch(const std::exception &error) {
        std::cerr << "std::exception: " << error.what() << std::endl;
    } catch(...) {
        std::cerr << "Abort after unknown exception" << std::endl;
    }
}
