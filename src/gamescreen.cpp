#include "gamescreen.h"

static const size_t SPRITES = 10000;

void GameScreen::Draw(Renderer &renderer)
{
    auto name = std::string("gm/tile_land3.gm1");

    renderer.BeginFrame();

    for(int x = 0; x < 1200 / TILE_RHOMBUS_WIDTH; ++x) {
        for(int y = 0; y < 768 / TILE_RHOMBUS_HEIGHT; ++y) {
            int voffset = (x % 2) * TILE_RHOMBUS_WIDTH / 2;
        }
    }
    
    renderer.EndFrame();
}

void GameScreen::OnMouseMotion(const SDL_MouseMotionEvent &)
{
}

void GameScreen::OnMouseButtonDown(const SDL_MouseButtonEvent &)
{
}

void GameScreen::OnMouseButtonUp(const SDL_MouseButtonEvent &)
{
}

void GameScreen::OnKeyDown(const SDL_KeyboardEvent &event)
{
    SDL_Log("Key down: %c", (char)event.keysym.sym);
}

void GameScreen::OnKeyUp(const SDL_KeyboardEvent &)
{
}

std::unique_ptr<Screen> GameScreen::NextScreen()
{
    return nullptr;
}
