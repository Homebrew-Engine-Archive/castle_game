#include "game.h"

Game::Game()
{
    m_closed = false;
}

Game::~Game()
{
}

bool Game::IsClosed() const
{
    return m_closed;
}

void Game::OnFrame(Renderer &renderer)
{
    if(m_frontscreen) {
        if(auto nextscreen = m_frontscreen->NextScreen()) {
            m_frontscreen = std::move(nextscreen);
        }
        m_frontscreen->OnFrame(renderer);
    }
}

void Game::OnEvent(const SDL_Event &event)
{
    switch(event.type) {
    case SDL_KEYDOWN:
        OnKeyDown(event.key);
        break;
    case SDL_KEYUP:
        OnKeyUp(event.key);
        break;
    case SDL_MOUSEBUTTONDOWN:
        OnMouseButtonDown(event.button);
        break;
    case SDL_MOUSEBUTTONUP:
        OnMouseButtonUp(event.button);
        break;
    case SDL_MOUSEMOTION:
        OnMouseMotion(event.motion);
        break;
    default:
        break;
    }
    if(m_frontscreen)
        m_frontscreen->OnEvent(event);
}

void Game::OnMouseMotion(const SDL_MouseMotionEvent &)
{
}

void Game::OnMouseButtonDown(const SDL_MouseButtonEvent &)
{
}

void Game::OnMouseButtonUp(const SDL_MouseButtonEvent &)
{
}

void Game::OnKeyDown(const SDL_KeyboardEvent &)
{
}

void Game::OnKeyUp(const SDL_KeyboardEvent &)
{
}
