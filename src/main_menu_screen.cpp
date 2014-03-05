#include "main_menu_screen.h"

MainMenuScreen::MainMenuScreen(RootScreen &root, Renderer &renderer)
    : m_root(root)
    , m_renderer(renderer)
    , m_background(renderer.QuerySurface("gfx/frontend_main.tgx"))
{
}

bool MainMenuScreen::HandleEvent(const SDL_Event &event)
{
    for(Button &button : m_buttons) {
        button.HandleEvent(event);
    }
    return false;
}

void MainMenuScreen::Draw()
{
    Surface frame = m_renderer.BeginFrame();
    
    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(m_background);    
    SDL_Rect bgAligned = AlignRect(bgRect, frameRect, 0, 0);

    SurfaceROI hud(frame, &bgAligned);
    for(Button &button : m_buttons) {
        button.Draw(hud);
    }
    
    BlitSurface(m_background, NULL, frame, &bgAligned);
    
    m_renderer.EndFrame();
}

void MainMenuScreen::GoMilitary()
{
}

void MainMenuScreen::GoEconomics()
{
}

void MainMenuScreen::GoMapEditor()
{
}

void MainMenuScreen::GoLoadGame()
{
}

void MainMenuScreen::Close()
{
}
