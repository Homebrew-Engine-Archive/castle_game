#ifndef INTERFACE_RENDERER_H_
#define INTERFACE_RENDERER_H_

class Rect;
class Surface;

namespace GM1
{
    class Palette;
}

namespace Render
{
    struct IRenderer
    {
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetScreenMode(int width, int height, int format) = 0;

        virtual void BindTexture(const Surface &surface) = 0;
        virtual void BindPalette(const GM1::Palette &palette) = 0;
        virtual void BindAlphaMap(const Surface &alphaMap) = 0;

        virtual void UnbindTexture() = 0;
        virtual void UnbindPalette() = 0;
        virtual void UnbindAlphaMap() = 0;

        virtual void BlitPalettizedTexture(const Rect &textureSubRect, const Rect &screenSubRect) = 0;
        virtual void BlitTexture(const Rect &textureSubRect, const Rect &screenSubRect) = 0;
        virtual void BlitAlphaMapped(const Rect &textureSubRect, const Rect &screenSubRect) = 0;
        
        virtual Surface CreateTexture(int width, int height, int format) = 0;
        virtual Surface CreateTextureFrom(char *data, int width, int height, int stride, int format) = 0;

    };
}

#endif // INTERFACE_RENDERER_H_
