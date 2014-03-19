#ifndef INCLUDE_RENDERER_H_
#define INCLUDE_RENDERER_H_

#include <string>
#include "SDL.h"
#include "text.h"
#include "sdl_utils.h"
#include "surface.h"
#include "collection.h"

class Renderer
{
private:
    struct RendererPimpl *m;
    
public:
    Renderer(SDL_Renderer *renderer);
    Renderer(const Renderer &that) = delete;
    Renderer &operator=(const Renderer &that) = delete;

    /**
     * Every frame should call this to create screen surface.
     */
    Surface BeginFrame();
    
    /**
     * Screen surface will be actually commited this.
     */
    void EndFrame();

    /**
     * Wrapper around SDL_GetRenderOutputSize.
     *
     * @return Rectangle of type (0, 0, w, h).
     */
    SDL_Rect GetOutputSize() const;

    /**
     * Check size of screen texture and reallocates it if necessary.
     *
     * @param width     Desired width.
     * @param height    Desired height.
     */
    void AdjustBufferSize(int width, int height);
    
    void RenderTextLine(const std::string &text, const SDL_Point &rect);

    void RenderTextBox(const std::string &text, const SDL_Rect &rect,
                       AlignH alignh, AlignV alignv);

    void SetFont(const std::string &fontname, font_size_t size);
    void SetColor(const SDL_Color &color);
    
    /**
     * This method queries surface from the cache and forward it
     * or try to load from filesystem.
     *
     * @param filename  Relative path from executable to *.TGX file
     * @return          Surface as it being stored in cache.
     */
    Surface QuerySurface(const FilePath &filename);

    /**
     * Queries collection from cache or force loads collection
     * from filesystem.
     * @param filename  Relative path from executable to *.GM1 file.
     * @return          Immutable reference to collection, owned by renderer.
     *
     */
    const CollectionData &QueryCollection(const FilePath &filename);
    
    /**
     * Force reloads collection.
     * @param filename  Relative path to file.
     * @return          True on success.
     */
    bool CacheCollection(const FilePath &filepath);

    bool CacheFontCollection(const FontCollectionInfo &info);
};

#endif
