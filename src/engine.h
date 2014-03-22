#ifndef ENGINE_H_
#define ENGINE_H_

#include <memory>
#include <SDL.h>
#include "renderer.h"
#include "screen.h"

namespace Castle
{

    struct Engine
    {
        virtual int Exec() = 0;
        virtual void SetCurrentScreen(ScreenPtr &&screen) = 0;
        virtual void PushScreen(ScreenPtr &&screen) = 0;
        virtual ScreenPtr PopScreen() = 0;
        virtual Renderer *GetRenderer() = 0;
        virtual bool Closed() const = 0;
        virtual void PollInput() = 0;
    };

    std::unique_ptr<Engine> CreateEngine(Renderer *renderer);
    
}

#endif
