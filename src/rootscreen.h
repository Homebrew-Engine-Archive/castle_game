#ifndef ROOTSCREEN_H_
#define ROOTSCREEN_H_

#include <memory>
#include "SDL.h"
#include "renderer.h"
#include "screen.h"

struct RootScreen
{
    virtual int Exec() = 0;
    virtual void DrawFrame() = 0;
    virtual bool HandleEvent(const SDL_Event &event) = 0;
    virtual void SetCurrentScreen(ScreenPtr &&screen) = 0;
    virtual void PushScreen(ScreenPtr &&screen) = 0;
    virtual ScreenPtr PopScreen() = 0;
    virtual Renderer *GetRenderer() = 0;
};

RootScreen *CreateRootScreen(Renderer *renderer);

#endif
