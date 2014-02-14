#include "gamescreen.h"

static const size_t WORK = 10000;

void GameScreen::Draw(SDLRenderer &renderer)
{
    auto name = std::string("gm/tile_land8.gm1");

    renderer.BeginFrame();
    for(size_t i = 0; i < WORK; ++i) {
        auto rect = renderer.QuerySurfaceRect(name, rand() % 100);

        SDL_Rect src = MakeRect(
            rand() % 1200, rand() % 1000,
            rect.w, rect.h);
        
        renderer.CopyDrawingPlain(name, &rect, &src);
    }
    renderer.EndFrame();
}

void GameScreen::OnMouseMotion(const SDL_MouseMotionEvent &event)
{
    (void)event;
}

void GameScreen::OnMouseButtonDown(const SDL_MouseButtonEvent &event)
{
    (void)event;    
}

void GameScreen::OnMouseButtonUp(const SDL_MouseButtonEvent &event)
{
    (void)event;
}

void GameScreen::OnKeyDown(const SDL_KeyboardEvent &event)
{
    SDL_Log("Key down: %c", (char)event.keysym.sym);
}

void GameScreen::OnKeyUp(const SDL_KeyboardEvent &event)
{
    (void)event;
}

std::unique_ptr<Screen> GameScreen::NextScreen()
{
    return nullptr;
}
