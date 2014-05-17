#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

namespace Render
{
    class Renderer;
    class FontManager;
}

namespace UI
{
    class ScreenManager;
}

namespace Castle
{
    class SimulationManager;
}

namespace Castle
{
    class Environment
    {
        Render::Renderer *mRenderer;
        UI::ScreenManager *mScreenManager;
        Render::FontManager *mFontManager;
        Castle::SimulationManager *mSimulationManager;
        
    public:
        void SetRenderer(Render::Renderer &renderer);
        void SetScreenManager(UI::ScreenManager &screenmanager);
        void SetFontManager(Render::FontManager &fontmanager);
        void SetSimulationManager(Castle::SimulationManager &simulationmanager);
      
        Render::Renderer& Renderer();
        UI::ScreenManager& ScreenManager();
        Render::FontManager& FontManager();
        Castle::SimulationManager& SimulationManager();
    };
}

#endif // ENVIRONMENT_H_
