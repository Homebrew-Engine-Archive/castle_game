#include "gamescreen.h"

static const size_t SPRITES = 10000;

GameScreen::GameScreen()
{
}

GameScreen::~GameScreen()
{
    SDL_Log("%08x::~GameScreen()", this);
}

void GameScreen::OnFrame(Renderer &renderer)
{
    auto name = std::string("gm/tile_land3.gm1");

    renderer.BeginFrame();

    renderer.EndFrame();
}

void GameScreen::OnEnterEventLoop()
{

}

void GameScreen::OnEvent(const SDL_Event &)
{
    
}

bool GameScreen::Closed() const
{
    return false;
}

std::unique_ptr<Screen> GameScreen::NextScreen()
{
    return nullptr;
}
