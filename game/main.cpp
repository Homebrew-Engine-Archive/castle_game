#include "main.h"

#include <SDL.h>

#include <vector>
#include <iostream>

#include <game/exception.h>
#include <game/sdl_utils.h>
#include <game/engine.h>
#include <game/renderer.h>

int main()
{
    const int windowWidth = 1024;
    const int windowHeight = 768;

    const int windowXPos = SDL_WINDOWPOS_UNDEFINED;
    const int windowYPos = SDL_WINDOWPOS_UNDEFINED;

    const char *windowTitle = "Stockade";

    const int windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    
    const int rendererIndex = -1;
    const int rendererFlags = SDL_RENDERER_ACCELERATED;
    
    WindowPtr sdlWindow;
    RendererPtr sdlRenderer;
    
    try {
        SDLInitializer init();
        
        sdlWindow.reset(
            SDL_CreateWindow(windowTitle,
                             windowXPos,
                             windowYPos,
                             windowWidth,
                             windowHeight,
                             windowFlags));

        if(!sdlWindow) {
            throw Castle::SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }

        sdlRenderer.reset(
            SDL_CreateRenderer(sdlWindow.get(),
                               rendererIndex,
                               rendererFlags));
        
        if(!sdlRenderer) {
            throw Castle::SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }

        std::unique_ptr<Render::Renderer> renderer;
        renderer.reset(new Render::Renderer(sdlRenderer.get()));

        std::unique_ptr<Castle::Engine> engine;
        engine.reset(new Castle::Engine(renderer.get()));

        int code = engine->Exec();
        std::clog << "Shutting down" << std::endl;
        return code;
        
    } catch(const Castle::Exception &error) {
        std::cerr << error << std::endl;
    } catch(const std::exception &error) {
        std::cerr << "std::exception: " << error.what() << std::endl;
    } catch(...) {
        std::cerr << "Abort after unknown exception" << std::endl;
    }
}
