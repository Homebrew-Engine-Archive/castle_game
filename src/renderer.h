#ifndef RENDERER_H_
#define RENDERER_H_

class Renderer;

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include "SDL.h"
#include <boost/noncopyable.hpp>

#include "sdl_utils.h"
#include "textrenderer.h"
#include "font.h"
#include "tgx.h"
#include "gm1.h"
#include "window.h"
#include "surface.h"
#include "rw.h"
#include "collection.h"

class Renderer : public boost::noncopyable
{
private:
    SDL_Renderer *m_renderer;
    TextRenderer m_textRenderer;
    
    // struct TextData
    // {
    //     std::string text;
    //     std::string fontname;
    //     SDL_Rect rect;
    //     SDL_Color color;
    //     font_size_t size;
    // };

    typedef std::function<void()> TextData;
    
    std::vector<TextData> m_textOverlay;
    void RenderTextOverlay(const TextData &text);
    
    int m_fbWidth;
    int m_fbHeight;
    Uint32 m_fbFormat;
    TexturePtr m_fbTexture;    
    bool ReallocationRequired(int width, int heigt);    
    bool AllocFrameTexture(int width, int height);

    Surface m_fbSurface;
    Surface AllocFrameSurface(void *pixels, int width, int height, int pitch);

    std::map<std::string, Surface> m_imageCache;
    Surface LoadSurface(const std::string &filename) const;

    std::map<std::string, CollectionDataPtr> m_cache;
    CollectionDataPtr LoadCollection(const std::string &filename) const;
    CollectionAtlasPtr LoadCollectionAtlas(const std::string &filename) const;
    
public:
    Renderer(SDL_Renderer *renderer);

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
    
    void RenderTextLine(const std::string &text, const SDL_Rect &rect);

    void SetFont(const std::string &fontname, font_size_t size);
    void SetColor(const SDL_Color &color);
    
    /**
     * This method queries surface from the cache and forward it
     * or try to load from filesystem.
     *
     * @param filename  Relative path from executable to *.TGX file
     * @return          Surface as it being stored in cache.
     */
    Surface QuerySurface(const std::string &filename);

    /**
     * Queries collection from cache or force loads collection
     * from filesystem.
     * @param filename  Relative path from executable to *.GM1 file.
     * @return          Immutable reference to collection, owned by renderer.
     *
     */
    const CollectionData &QueryCollection(const std::string &filename);
    
    /**
     * Force reloads collection.
     * @param filename  Relative path to file.
     * @return          True on success.
     */
    bool CacheCollection(const std::string &filename);

    bool CacheFontCollection(const FontCollectionInfo &info);
};

void EnumRenderDrivers();
void PrintRendererInfo(SDL_Renderer *renderer);

#endif
