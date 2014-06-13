#ifndef TEXTURE_H_
#define TEXTURE_H_

class Surface;
class Rect;

namespace Render
{
    class Texture
    {
    public:
        virtual void Copy(const Surface &sourceData, const Rect &source, const Rect &target) = 0;
        virtual void Draw(const Rect &source, const Rect &target) = 0;
    };
}

#endif // TEXTURE_H_
