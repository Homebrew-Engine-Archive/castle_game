#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <game/vmath.h>

class Rect;
class Color;
class Surface;

namespace GM1
{
    class Palette;
}

namespace Render
{
    class Renderer;
}

namespace Render
{
    class ScenePrivate;
    class SceneItem;
    
    class Scene
    {
    public:
        explicit Scene(Renderer&);
        Scene(const Scene &scene);
        Scene& operator=(const Scene &scene);
        ~Scene();

        void Translated(const core::Vec2i &delta);
        void Scaled(const core::Vec2f &delta);
        void Clipped(const core::Vec2i &delta);
        void Blended(int alpha);

        void SetDrawColor(const Color &color);
        void SetBackColor(const Color &color);
        
        void DrawText(const std::string &text, const Color &color);
        void DrawSprite(const Surface &surface, const GM1::Palette &palette, const Rect &subrect);
        void DrawAlpha(const Surface &surface, const Surface &alphaMap, const Rect &subrect);
        void DrawSurface(const Surface &surface, const Rect &subrect);

        void Render(Render::Renderer &renderer);
        
    protected:
        std::shared_ptr<ScenePrivate> pImpl;
    };
}

#endif // SCENE_H_
