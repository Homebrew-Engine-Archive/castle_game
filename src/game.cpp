#include "game.h"

Game::Game()
{
    m_CurrentMouseInvalid = true;
    m_CurrentMouseX = 0;
    m_CurrentMouseY = 0;

    m_ActiveScreen = std::unique_ptr<GameScreen>(
        new GameScreen);

    m_Closed = false;
}

Game::~Game()
{
}

bool Game::IsClosed() const
{
    return m_Closed;
}

void Game::OnQuit()
{
}

void Game::OnFrame(Renderer &renderer)
{
    m_ActiveScreen->Draw(renderer);
}

void Game::OnMouseMotion(const SDL_MouseMotionEvent &event)
{
    m_CurrentMouseX = event.x;
    m_CurrentMouseY = event.y;
    m_CurrentMouseInvalid = false;
}

void Game::OnMouseButtonDown(const SDL_MouseButtonEvent &event)
{
    m_ActiveScreen->OnMouseButtonDown(event);
}

void Game::OnMouseButtonUp(const SDL_MouseButtonEvent &event)
{
    m_ActiveScreen->OnMouseButtonUp(event);
}

void Game::OnKeyDown(const SDL_KeyboardEvent &event)
{
    if(event.keysym.sym == SDLK_ESCAPE)
        m_Closed = true;
    m_ActiveScreen->OnKeyDown(event);
}

void Game::OnKeyUp(const SDL_KeyboardEvent &event)
{
    m_ActiveScreen->OnKeyUp(event);
}

void Game::OnTimeElapsed(Uint32)
{
    
}
