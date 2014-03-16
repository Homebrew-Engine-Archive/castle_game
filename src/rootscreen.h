#ifndef ROOTSCREEN_H_
#define ROOTSCREEN_H_

class RootScreen;

#include <memory>
#include "SDL.h"
#include "renderer.h"
#include "screen.h"

class RootScreen
{
private:
    struct RootScreenPimpl *m;
    
public:
    RootScreen(Renderer *renderer);
    RootScreen(RootScreen &&that) = default;
    RootScreen(const RootScreen &that) = delete;
    RootScreen &operator=(const RootScreen &that) = delete;
    RootScreen &operator=(RootScreen &&that) = default;
    ~RootScreen();

    int Exec();

    void DrawFrame();

    bool HandleEvent(const SDL_Event &event);

    void SetCurrentScreen(std::unique_ptr<Screen> &&screen);
    void PushScreen(std::unique_ptr<Screen> &&screen);

    Renderer *GetRenderer();
};

#endif
