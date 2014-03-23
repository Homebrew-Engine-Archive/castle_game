#ifndef ENGINE_H_
#define ENGINE_H_

#include <memory>
#include <SDL.h>
#include "screen.h"

namespace Render
{
    class Renderer;
}

namespace Castle
{

    class Engine
    {
    public:
        virtual int Exec() = 0;
        virtual void SetCurrentScreen(GUI::ScreenPtr &&screen) = 0;
        virtual void PushScreen(GUI::ScreenPtr &&screen) = 0;
        virtual GUI::ScreenPtr PopScreen() = 0;
        virtual Render::Renderer *GetRenderer() = 0;
        virtual bool Closed() const = 0;
        virtual void PollInput() = 0;
    };

    std::unique_ptr<Engine> CreateEngine(Render::Renderer *renderer);
    
}

#endif
