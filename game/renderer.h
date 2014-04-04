#ifndef RENDERER_H_
#define RENDERER_H_

#include <string>
#include <SDL.h>
#include <game/filesystem.h>

enum class AlignH;
enum class AlignV;
class Text;

class CollectionData;
class FontCollectionInfo;
class Surface;

namespace Render
{

    class Renderer
    {
    public:
        virtual ~Renderer() {}
        virtual Surface BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual SDL_Rect GetOutputSize() const = 0;
        virtual void AdjustBufferSize(int width, int height) = 0;
        virtual void RenderTextLine(const std::string &text, const SDL_Point &point) = 0;
        virtual void RenderTextBox(const std::string &text, const SDL_Rect &rect, AlignH alignh, AlignV alignv) = 0;
        virtual void SetFont(const std::string &fontname, int size) = 0;
        virtual void SetColor(const SDL_Color &color) = 0;
        virtual Surface QuerySurface(const FilePath &filename) = 0;
        virtual const CollectionData &QueryCollection(const FilePath &filename) = 0;
        virtual bool CacheCollection(const FilePath &filepath) = 0;
        virtual bool CacheFontCollection(const FontCollectionInfo &info) = 0;
    };

    std::unique_ptr<Renderer> CreateRenderer(SDL_Renderer *renderer);
    
} // namespace Render

#endif
