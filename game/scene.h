#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <game/vmath.h>

class Color;
class Surface;

namespace GM1
{
    class Palette;
}

namespace Render
{
    class FontManager;
    class Renderer;
}

namespace Render
{
    class SceneItem;
    
    class Scene
    {
    public:
        explicit Scene(FontManager&, Renderer&);
        Scene(const Scene &scene);
        Scene& operator=(const Scene &scene);
        ~Scene();
        
        void Translate(const core::Vec2i &delta);
        void Untranslate();
        
        void Scale(const core::Vec2f &delta);
        void Unscale();
        
        void Crop(const core::Vec2i &delta);
        void Uncrop();

        void Alpha(int alpha);
        void Unalpha();

        void DrawText(const std::string &text, const Color &color);
        void DrawSprite(const Surface &surface, const GM1::Palette &palette);
        void DrawSurface(const Surface &surface);

        void Render();
        
    protected:
        std::vector<core::Vec2i> mTranslationStack;
        core::Vec2i mTranslation;
        std::vector<core::Vec2f> mScaleStack;
        core::Vec2f mScale;
        std::vector<float> mAlphaStack;
        int mAlpha;
        std::vector<core::Vec2i> mCropStack;
        core::Vec2i mCrop;
        std::vector<SceneItem> mSceneItems;
    };
}

#endif // SCENE_H_
