#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <stdexcept>
#include <set>
#include <map>
#include <vector>
#include <string>
#include "SDL.h"

#include <boost/noncopyable.hpp>

class Renderer;

#include "textrenderer.h"
#include "font.h"
#include "streamingtexture.h"
#include "tgx.h"
#include "gm1.h"
#include "window.h"
#include "surface.h"
#include "rw.h"

struct DestroyRendererDeleter
{
    void operator()(SDL_Renderer *renderer) const {
        if(renderer != NULL) {
            SDL_DestroyRenderer(renderer);
        }
    };
};

typedef std::unique_ptr<SDL_Renderer, DestroyRendererDeleter> RendererPtr;

struct DestroyTextureDeleter
{
    void operator()(SDL_Texture *texture) const {
        if(texture != NULL) {
            SDL_DestroyTexture(texture);
        }
    };
};

typedef std::unique_ptr<SDL_Texture, DestroyTextureDeleter> TexturePtr;

struct CollectionEntry
{
    gm1::ImageHeader header;
    Surface surface;
    CollectionEntry(const gm1::ImageHeader &hdr_, const Surface &sf_)
        : header(hdr_)
        , surface(sf_)
        {}
};

struct CollectionData
{
    gm1::Header header;
    std::vector<CollectionEntry> entries;
    std::vector<SDL_Palette *> palettes;
};

class Renderer : public boost::noncopyable
{
private:
    RendererPtr m_renderer;
    TextRenderer m_textRenderer;
    
    struct TextData
    {
        std::string text;
        std::string fontname;
        SDL_Rect rect;
        SDL_Color color;
    };
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
    Surface LoadSurface(const std::string &filename);

    std::map<std::string, CollectionData> m_cache;
    
public:
    Renderer(Window *window);

    inline SDL_Renderer *GetRenderer() { return m_renderer.get(); }
    
    Surface BeginFrame();
    void EndFrame();

    SDL_Rect GetOutputSize() const;
    void AdjustOutputSize(int width, int height);

    void RenderTextLine(
        const std::string &text,
        const SDL_Rect *rect,
        const std::string &fontname,
        const SDL_Color &color,
        font_size_t size);
    
    Surface QuerySurface(const std::string &filename);

    bool LoadImageCollection(const std::string &filename);
    const CollectionData &GetCollection(const std::string &filename) const;

    void LoadFont(const std::string &name, const std::string &filename);
};

inline static std::string FontStronghold()
{
    return "stronghold";
}

inline static std::string FontSlanted()
{
    return "slanted";
}

inline static std::string FontStrongholdAA()
{
    return "stronghold_aa";
}

SDL_Color MakeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void EnumRenderDrivers();
void PrintRendererInfo(SDL_Renderer *renderer);

#endif
