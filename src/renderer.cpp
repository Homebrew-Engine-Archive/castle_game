#include <map>
#include <vector>
#include <functional>
#include "renderer.h"
#include "textrenderer.h"
#include "geometry.h"
#include <boost/algorithm/clamp.hpp>

typedef std::function<void()> TextBatch;

namespace
{

    const int MIN_OUTPUT_WIDTH = 320;
    const int MIN_OUTPUT_HEIGHT = 240;
    
    // TODO sync with driver's texture max width and height
    const int MAX_OUTPUT_WIDTH = 4096;
    const int MAX_OUTPUT_HEIGHT = 4096;
    
}

struct RendererPimpl
{   
    SDL_Renderer *renderer;
    TextRenderer textRenderer;
    int fbWidth;
    int fbHeight;
    Uint32 fbFormat;
    TexturePtr fbTexture;
    Surface fbSurface;
    std::vector<TextBatch> textOverlay;
    std::map<FilePath, Surface> tgxCache;
    std::map<FilePath, CollectionDataPtr> gm1Cache;

    RendererPimpl(SDL_Renderer *renderer);

    int AdjustWidth(int width) const;
    int AdjustHeight(int height) const;

    bool ReallocationRequired(int width, int heigth);
    bool CreateFrameTexture(int width, int height);
    bool CreateFrameSurface(void *pixels, int width, int height, int pitch);
};

RendererPimpl::RendererPimpl(SDL_Renderer *renderer_)
    : renderer(renderer_)
    , textRenderer(renderer_)
    , fbWidth(0)
    , fbHeight(0)
    , fbFormat(0)
{ }

int RendererPimpl::AdjustWidth(int width) const
{
    return boost::algorithm::clamp(width, MIN_OUTPUT_WIDTH, MAX_OUTPUT_WIDTH);
}

int RendererPimpl::AdjustHeight(int height) const
{
    return boost::algorithm::clamp(height, MIN_OUTPUT_HEIGHT, MAX_OUTPUT_HEIGHT);
}

bool RendererPimpl::CreateFrameTexture(int width, int height)
{
    // NOTE
    // Width and height wan't be checked for min and max constraints.
    // It's assumed that they already being checked.
    if((width == 0) || (height == 0)) {
        std::cerr << "Size is zero. Abort allocation."
                  << std::endl;
        return false;
    }
    
    if((fbTexture) && (width == fbWidth) && (height == fbHeight)) {
        std::cerr << "Size of texture matches. Skip allocation."
                  << std::endl;
        return true;
    }
    
    fbWidth = width;
    fbHeight = height;
    fbFormat = SDL_PIXELFORMAT_ARGB8888;
    fbTexture =
        TexturePtr(
            SDL_CreateTexture(
                renderer,
                fbFormat,
                SDL_TEXTUREACCESS_STREAMING,
                fbWidth,
                fbHeight));

    if(!fbTexture) {
        std::cerr << "SDL_CreateTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return false;
    }
    
    return true;
}

bool RendererPimpl::CreateFrameSurface(void *pixels, int width, int height, int pitch)
{
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;
    if(!SDL_PixelFormatEnumToMasks(fbFormat, &bpp, &rmask, &gmask, &bmask, &amask)) {
        std::cerr << "SDL_PixelFormatEnumToMasks failed: "
                  << SDL_GetError()
                  << std::endl;
        return false;
    }

    Surface surface = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
    if(surface.Null()) {
        std::cerr << "SDL_CreateRGBSurfaceFrom failed: "
                  << SDL_GetError()
                  << std::endl;
        return false;
    }

    fbSurface = surface;
    
    return true;
}

bool RendererPimpl::ReallocationRequired(int width, int height)
{
    return (width != fbWidth) || (height != fbHeight);
}

Renderer::Renderer(SDL_Renderer *renderer)
    : m {new RendererPimpl(renderer)}
{
    SDL_Rect rect = GetOutputSize();
    m->fbWidth = rect.w;
    m->fbHeight = rect.h;
}

Surface Renderer::BeginFrame()
{
    if(!m->textOverlay.empty()) {
        std::clog << "Discard text overlay which has "
                  << std::dec << m->textOverlay.size()
                  << std::endl;
        m->textOverlay.clear();
    }

    if(!m->fbSurface.Null()) {
        std::cerr << "Previously allocated surface is not null"
                  << std::endl;
        m->fbSurface.reset();
    }
    
    if(!m->fbTexture) {
        if(!m->CreateFrameTexture(m->fbWidth, m->fbHeight)) {
            std::cerr << "Can't allocate frame texture"
                      << std::endl;
            return Surface();
        }
    }
    
    int nativePitch;
    void *nativePixels;
    if(SDL_LockTexture(m->fbTexture.get(), NULL, &nativePixels, &nativePitch)) {
        std::cerr << "SDL_LockTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }
        
    if(!m->CreateFrameSurface(nativePixels, m->fbWidth, m->fbHeight, nativePitch)) {
        std::cerr << "Can't allocate framebuffer"
                  << std::endl;
        SDL_UnlockTexture(m->fbTexture.get());
        return Surface();
    }

    return m->fbSurface;
}

void Renderer::EndFrame()
{
    if(!m->fbSurface.Null()) {
        SDL_RenderClear(m->renderer);

        // NOTE
        // It wan't deallocate pixels, only surface object
        m->fbSurface.reset();
        
        SDL_UnlockTexture(m->fbTexture.get());
        if(SDL_RenderCopy(m->renderer, m->fbTexture.get(), NULL, NULL)) {
            std::cerr << "SDL_RenderCopy failed: "
                      << SDL_GetError()
                      << std::endl;
        }
    }

    if(!m->textOverlay.empty()) {
        for(TextBatch batch : m->textOverlay) {
            batch();
        }
        m->textOverlay.clear();
    }
    
    SDL_RenderPresent(m->renderer);
}

SDL_Rect Renderer::GetOutputSize() const
{
    int width;
    int height;
    if(SDL_GetRendererOutputSize(m->renderer, &width, &height)) {
        std::cerr << "SDL_GetRendererOutputSize failed: "
                  << SDL_GetError()
                  << std::endl;
    }
    return MakeRect(width, height);
}

void Renderer::AdjustBufferSize(int width, int height)
{
    if(!m->fbSurface.Null()) {
        throw std::runtime_error("AdjustBufferSize called with active frame.");
    }

    // Cutting up and down texture height and width
    int adjustedWidth = m->AdjustWidth(width);
    int adjustedHeight = m->AdjustHeight(height);

    // NOTE
    // This is the only place we limit width and height
    if(m->ReallocationRequired(adjustedWidth, adjustedHeight)) {
        m->CreateFrameTexture(adjustedWidth, adjustedHeight);
    }
}

Surface Renderer::QuerySurface(const FilePath &filename)
{
    auto cached = m->tgxCache.find(filename);
    if(cached != m->tgxCache.end()) {
        return cached->second;
    } else {
        Surface loaded = LoadSurface(filename);
        m->tgxCache.insert({filename, loaded});
        return loaded;
    }
}

const CollectionData &Renderer::QueryCollection(const FilePath &filename)
{
    auto searchResult = m->gm1Cache.find(filename);
    if(searchResult != m->gm1Cache.end()) {
        return *searchResult->second;
    } else {
        CollectionDataPtr ptr =
            std::move(
                LoadCollectionData(filename));

        if(!ptr)
            throw std::runtime_error("Unable to load collection");
        
        const CollectionData &data = *ptr;
        m->gm1Cache.insert(
            std::make_pair(filename, std::move(ptr)));

        return data;
    }
}

bool Renderer::CacheCollection(const FilePath &filename)
{
    try {
        QueryCollection(filename);
        return true;
    } catch(const std::exception &e) {
        std::cerr << "Cache collection failed"
                  << std::endl;
        return false;
    }
}

bool Renderer::CacheFontCollection(const FontCollectionInfo &info)
{
    try {
        CollectionDataPtr data =
            std::move(
                LoadCollectionData(info.filename));
        if(!data)
            throw std::runtime_error("Unable load font collection");
        
        size_t skip = 0;
        for(font_size_t size : info.sizes) {
            Font font(*data, info.alphabet, skip);
            if(!m->textRenderer.CacheFont(info.name, size, font)) {
                std::cerr << "Unable to cache font: "
                          << info.name << ' '
                          << size << std::endl;
            }
            skip += info.alphabet.size();
        }
        
        return true;
    } catch(const std::exception &e) {
        std::cerr << "Exception in CacheFontCollection: "
                  << e.what()
                  << std::endl;
        return false;
    }
}

void Renderer::SetFont(const std::string &fontname, font_size_t size)
{
    auto changeFont = [fontname, size, this]() {
        m->textRenderer.SetFont(fontname, size);
    };
    m->textOverlay.push_back(changeFont);
}

void Renderer::SetColor(const SDL_Color &color)
{
    auto changeColor = [color, this]() {
        m->textRenderer.SetColor(color);
    };
    m->textOverlay.push_back(changeColor);
}

void Renderer::RenderTextLine(const std::string &text, const SDL_Point &point)
{
    auto drawText = [text, point, this]() {
        SDL_Rect textRect = m->textRenderer.CalculateTextRect(text);
    
        m->textRenderer.SetCursor(
            ShiftPoint(point, 0, textRect.h));
    
        m->textRenderer.PutString(text);
    };
    
    m->textOverlay.push_back(drawText);
}

void Renderer::RenderTextBox(const std::string &text, const SDL_Rect &rect,
                             AlignH alignh, AlignV alignv)
{
    UNUSED(alignh);
    UNUSED(alignv);
    RenderTextLine(text, TopLeft(rect));
}
