#include "environment.h"

namespace Castle
{
    Render::Renderer& Environment::Renderer()
    {
        return *mRenderer;
    }

    Render::FontManager& Environment::FontManager()
    {
        return *mFontManager;
    }

    UI::ScreenManager& Environment::ScreenManager()
    {
        return *mScreenManager;
    }

    Castle::SimulationManager& Environment::SimulationManager()
    {
        return *mSimulationManager;
    }

    void Environment::SetRenderer(Render::Renderer &renderer)
    {
        mRenderer = &renderer;
    }
    
    void Environment::SetScreenManager(UI::ScreenManager &screenmanager)
    {
        mScreenManager = &screenmanager;
    }
    
    void Environment::SetFontManager(Render::FontManager &fontmanager)
    {
        mFontManager = &fontmanager;
    }
    
    void Environment::SetSimulationManager(Castle::SimulationManager &simulationmanager)
    {
        mSimulationManager = &simulationmanager;
    }
}
