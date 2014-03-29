#ifndef WIDGET_H_
#define WIDGET_H_

#include <SDL.h>

class Surface;

namespace Render
{
    class GraphicsManager;
}

namespace UI
{
    void LoadGraphics(Render::GraphicsManager const&);
    
    class Widget
    {
    public:
        virtual void HandleEvent(const SDL_Event &event) = 0;
        virtual void Draw(Surface &surface) = 0;
        virtual SDL_Rect Rect() const = 0;
    };

}

#endif
