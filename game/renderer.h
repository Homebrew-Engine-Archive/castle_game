#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>

#include <SDL.h>

#include <game/collection.h>
#include <game/surface.h>
#include <game/sdl_utils.h>
#include <game/filesystem.h>

enum class AlignH;
enum class AlignV;
class Text;

class CollectionData;
class FontCollectionInfo;
class Surface;

namespace Render
{
    class TextRenderer;
}

namespace Render
{

    class Renderer
    {
        typedef std::function<void()> TextBatch;
        
        SDL_Renderer *mRenderer;
        std::unique_ptr<TextRenderer> mTextRenderer;
        int mBufferWidth;
        int mBufferHeight;
        uint32_t mFbFormat;
        TexturePtr mBuffTexture;
        Surface mBuffSurface;
        std::vector<TextBatch> mTextOverlay;
        std::map<FilePath, Surface> mGFXCache;
        std::map<FilePath, CollectionDataPtr> mGMCache;

        bool ReallocationRequired(int width, int heigth);
        bool CreateFrameTexture(int width, int height);
        bool CreateFrameSurface(void *pixels, int width, int height, int pitch);

    public:
        Renderer();
        Renderer(SDL_Renderer *renderer);
        Renderer(const Renderer &);
        Renderer(Renderer &&);
        Renderer &operator=(const Renderer &);
        Renderer &operator=(Renderer &&);
        virtual ~Renderer();

        Surface BeginFrame();
        void EndFrame();
        SDL_Rect GetOutputSize() const;
        void AdjustBufferSize(int width, int height);
        void RenderTextLine(const std::string &text, const SDL_Point &rect);
        void RenderTextBox(const std::string &text, const SDL_Rect &rect, AlignH alignh, AlignV alignv);
        void SetFont(const std::string &fontname, int size);
        void SetColor(const SDL_Color &color);
        Surface QuerySurface(const FilePath &filename);
        const CollectionData &QueryCollection(const FilePath &filename);
        bool CacheCollection(const FilePath &filepath);
        bool CacheFontCollection(const FontCollectionInfo &info);
    };
    
} // namespace Render

#endif
