#include "renderer.h"
#include "text.h"
#include "textrenderer.h"
#include "font.h"
#include "geometry.h"
#include "collection.h"
#include <map>
#include <vector>
#include <functional>
#include <boost/algorithm/clamp.hpp>

namespace
{
    
    typedef std::function<void()> TextBatch;
    
    const int MIN_OUTPUT_WIDTH = 320;
    const int MIN_OUTPUT_HEIGHT = 240;
    
    // TODO sync with driver's texture max width and height
    const int MAX_OUTPUT_WIDTH = 4096;
    const int MAX_OUTPUT_HEIGHT = 4096;
    
}

class RendererImpl final : public Renderer
{   
    SDL_Renderer *m_renderer;
    std::unique_ptr<TextRenderer> m_textRenderer;
    int m_fbWidth;
    int m_fbHeight;
    Uint32 m_fbFormat;
    TexturePtr m_fbTexture;
    Surface m_fbSurface;
    std::vector<TextBatch> m_textOverlay;
    std::map<FilePath, Surface> m_tgxCache;
    std::map<FilePath, CollectionDataPtr> m_gm1Cache;
        
    int AdjustWidth(int width) const;
    int AdjustHeight(int height) const;
    bool ReallocationRequired(int width, int heigth);
    bool CreateFrameTexture(int width, int height);
    bool CreateFrameSurface(void *pixels, int width, int height, int pitch);

public:
    RendererImpl(SDL_Renderer *renderer);
    RendererImpl(const RendererImpl &) = delete;
    RendererImpl(RendererImpl &&) = default;
    RendererImpl &operator=(const RendererImpl &) = delete;
    RendererImpl &operator=(RendererImpl &&) = default;

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

RendererImpl::RendererImpl(SDL_Renderer *renderer)
    : m_renderer(renderer)
    , m_textRenderer(std::move(CreateTextRenderer(renderer)))
    , m_fbWidth(0)
    , m_fbHeight(0)
    , m_fbFormat(0)
    , m_fbSurface(NULL)
{
    SDL_Rect rect = GetOutputSize();
    m_fbWidth = rect.w;
    m_fbHeight = rect.h;

    std::clog << "GetOutputSize(): " << rect << std::endl;
}

int RendererImpl::AdjustWidth(int width) const
{
    return boost::algorithm::clamp(width, MIN_OUTPUT_WIDTH, MAX_OUTPUT_WIDTH);
}

int RendererImpl::AdjustHeight(int height) const
{
    return boost::algorithm::clamp(height, MIN_OUTPUT_HEIGHT, MAX_OUTPUT_HEIGHT);
}

bool RendererImpl::CreateFrameTexture(int width, int height)
{
    // NOTE
    // Width and height wan't be checked for min and max constraints.
    // It's assumed that they already being checked.
    if((width == 0) || (height == 0)) {
        std::cerr << "Size is zero. Abort allocation."
                  << std::endl;
        return false;
    }
    
    if((m_fbTexture) && (width == m_fbWidth) && (height == m_fbHeight)) {
        std::cerr << "Size of texture matches. Skip allocation."
                  << std::endl;
        return true;
    }
    
    m_fbWidth = width;
    m_fbHeight = height;
    m_fbFormat = SDL_PIXELFORMAT_ARGB8888;
    m_fbTexture =
        TexturePtr(
            SDL_CreateTexture(
                m_renderer,
                m_fbFormat,
                SDL_TEXTUREACCESS_STREAMING,
                m_fbWidth,
                m_fbHeight));

    if(!m_fbTexture) {
        std::cerr << "SDL_CreateTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return false;
    }
    
    return true;
}

bool RendererImpl::CreateFrameSurface(void *pixels, int width, int height, int pitch)
{
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;
    if(!SDL_PixelFormatEnumToMasks(m_fbFormat, &bpp, &rmask, &gmask, &bmask, &amask)) {
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

    m_fbSurface = surface;
    
    return true;
}

bool RendererImpl::ReallocationRequired(int width, int height)
{
    return (width != m_fbWidth) || (height != m_fbHeight);
}
    
Surface RendererImpl::BeginFrame()
{
    if(!m_textOverlay.empty()) {
        std::clog << "Discard text overlay which has "
                  << std::dec << m_textOverlay.size()
                  << std::endl;
        m_textOverlay.clear();
    }

    if(!m_fbSurface.Null()) {
        std::cerr << "Previously allocated surface is not null"
                  << std::endl;
        m_fbSurface.reset();
    }
    
    if(!m_fbTexture) {
        if(!CreateFrameTexture(m_fbWidth, m_fbHeight)) {
            std::cerr << "Can't allocate frame texture"
                      << std::endl;
            return Surface();
        }
    }
    
    int nativePitch;
    void *nativePixels;
    if(SDL_LockTexture(m_fbTexture.get(), NULL, &nativePixels, &nativePitch)) {
        std::cerr << "SDL_LockTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }
        
    if(!CreateFrameSurface(nativePixels, m_fbWidth, m_fbHeight, nativePitch)) {
        std::cerr << "Can't allocate framebuffer"
                  << std::endl;
        SDL_UnlockTexture(m_fbTexture.get());
        return Surface();
    }

    return m_fbSurface;
}

void RendererImpl::EndFrame()
{
    if(!m_fbSurface.Null()) {
        SDL_RenderClear(m_renderer);

        // NOTE
        // It wan't deallocate pixels, only surface object
        m_fbSurface.reset();
        
        SDL_UnlockTexture(m_fbTexture.get());
        if(SDL_RenderCopy(m_renderer, m_fbTexture.get(), NULL, NULL)) {
            std::cerr << "SDL_RenderCopy failed: "
                      << SDL_GetError()
                      << std::endl;
        }
    }

    if(!m_textOverlay.empty()) {
        for(TextBatch batch : m_textOverlay) {
            batch();
        }
        m_textOverlay.clear();
    }
    
    SDL_RenderPresent(m_renderer);
}

SDL_Rect RendererImpl::GetOutputSize() const
{
    int width;
    int height;
    if(SDL_GetRendererOutputSize(m_renderer, &width, &height)) {
        std::cerr << "SDL_GetRendererOutputSize failed: "
                  << SDL_GetError()
                  << std::endl;
    }
    return MakeRect(width, height);
}

void RendererImpl::AdjustBufferSize(int width, int height)
{
    if(!m_fbSurface.Null()) {
        throw std::runtime_error("AdjustBufferSize called with active frame.");
    }

    // Cutting up and down texture height and width
    int adjustedWidth = AdjustWidth(width);
    int adjustedHeight = AdjustHeight(height);

    // NOTE
    // This is the only place we limit width and height
    if(ReallocationRequired(adjustedWidth, adjustedHeight)) {
        std::clog << "AdjustBufferSize(): " << std::dec
                  << MakeRect(adjustedWidth, adjustedHeight)
                  << std::endl;
        CreateFrameTexture(adjustedWidth, adjustedHeight);
    }
}

Surface RendererImpl::QuerySurface(const FilePath &filename)
{
    auto cached = m_tgxCache.find(filename);
    if(cached != m_tgxCache.end()) {
        return cached->second;
    } else {
        Surface loaded = LoadSurface(filename);
        m_tgxCache.insert({filename, loaded});
        return loaded;
    }
}

const CollectionData &RendererImpl::QueryCollection(const FilePath &filename)
{
    auto searchResult = m_gm1Cache.find(filename);
    if(searchResult != m_gm1Cache.end()) {
        return *searchResult->second;
    } else {
        CollectionDataPtr ptr =
            std::move(
                LoadCollectionData(filename));

        if(!ptr)
            throw std::runtime_error("Unable to load collection");
        
        const CollectionData &data = *ptr;
        m_gm1Cache.insert(
            std::make_pair(filename, std::move(ptr)));

        return data;
    }
}

bool RendererImpl::CacheCollection(const FilePath &filename)
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

bool RendererImpl::CacheFontCollection(const FontCollectionInfo &info)
{
    try {
        CollectionDataPtr data =
            std::move(
                LoadCollectionData(info.filename));
        if(!data)
            throw std::runtime_error("Unable load font collection");
        
        size_t skip = 0;
        for(int fontSize : info.sizes) {
            Font font = MakeFont(*data, info.alphabet, skip);
            if(!m_textRenderer->CacheFont(info.name, fontSize, font)) {
                std::cerr << "Unable to cache font: "
                          << info.name << ' '
                          << fontSize << std::endl;
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

void RendererImpl::SetFont(const std::string &fontname, int size)
{
    auto changeFont = [fontname, size, this]() {
        m_textRenderer->SetFont(fontname, size);
    };
    m_textOverlay.push_back(changeFont);
}

void RendererImpl::SetColor(const SDL_Color &color)
{
    auto changeColor = [color, this]() {
        m_textRenderer->SetColor(color);
    };
    m_textOverlay.push_back(changeColor);
}

void RendererImpl::RenderTextLine(const std::string &text, const SDL_Point &point)
{
    auto drawText = [text, point, this]() {
        SDL_Rect textRect = m_textRenderer->CalculateTextRect(text);
    
        m_textRenderer->SetCursor(
            ShiftPoint(point, 0, textRect.h));
    
        m_textRenderer->PutString(text);
    };
    
    m_textOverlay.push_back(drawText);
}

void RendererImpl::RenderTextBox(const std::string &text, const SDL_Rect &rect,
                             AlignH alignh, AlignV alignv)
{
    UNUSED(alignh);
    UNUSED(alignv);
    RenderTextLine(text, TopLeft(rect));
}
    
std::unique_ptr<Renderer> CreateRenderer(SDL_Renderer *renderer)
{
    return make_unique<RendererImpl>(renderer);
}
