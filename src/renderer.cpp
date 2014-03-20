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
    SDL_Renderer *mRenderer;
    std::unique_ptr<TextRenderer> mTextRenderer;
    int mFbWidth;
    int mFbHeight;
    Uint32 mFbFormat;
    TexturePtr mFbTexture;
    Surface mFbSurface;
    std::vector<TextBatch> mTextOverlay;
    std::map<FilePath, Surface> mTgxCache;
    std::map<FilePath, CollectionDataPtr> mGm1Cache;
        
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
    : mRenderer(renderer)
    , mTextRenderer(std::move(CreateTextRenderer(renderer)))
    , mFbWidth(0)
    , mFbHeight(0)
    , mFbFormat(0)
    , mFbSurface(NULL)
{
    SDL_Rect rect = GetOutputSize();
    mFbWidth = rect.w;
    mFbHeight = rect.h;

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
    
    if((mFbTexture) && (width == m_fbWidth) && (height == m_fbHeight)) {
        std::cerr << "Size of texture matches. Skip allocation."
                  << std::endl;
        return true;
    }
    
    mFbWidth = width;
    mFbHeight = height;
    mFbFormat = SDL_PIXELFORMAT_ARGB8888;
    mFbTexture =
        TexturePtr(
            SDL_CreateTexture(
                mRenderer,
                mFbFormat,
                SDL_TEXTUREACCESS_STREAMING,
                mFbWidth,
                mFbHeight));

    if(!mFbTexture) {
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
    if(!SDL_PixelFormatEnumToMasks(mFbFormat, &bpp, &rmask, &gmask, &bmask, &amask)) {
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

    mFbSurface = surface;
    
    return true;
}

bool RendererImpl::ReallocationRequired(int width, int height)
{
    return (width != mFbWidth) || (height != m_fbHeight);
}
    
Surface RendererImpl::BeginFrame()
{
    if(!mTextOverlay.empty()) {
        std::clog << "Discard text overlay which has "
                  << std::dec << mTextOverlay.size()
                  << std::endl;
        mTextOverlay.clear();
    }

    if(!mFbSurface.Null()) {
        std::cerr << "Previously allocated surface is not null"
                  << std::endl;
        mFbSurface.reset();
    }
    
    if(!mFbTexture) {
        if(!CreateFrameTexture(mFbWidth, m_fbHeight)) {
            std::cerr << "Can't allocate frame texture"
                      << std::endl;
            return Surface();
        }
    }
    
    int nativePitch;
    void *nativePixels;
    if(SDL_LockTexture(mFbTexture.get(), NULL, &nativePixels, &nativePitch)) {
        std::cerr << "SDL_LockTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }
        
    if(!CreateFrameSurface(nativePixels, mFbWidth, m_fbHeight, nativePitch)) {
        std::cerr << "Can't allocate framebuffer"
                  << std::endl;
        SDL_UnlockTexture(mFbTexture.get());
        return Surface();
    }

    return mFbSurface;
}

void RendererImpl::EndFrame()
{
    if(!mFbSurface.Null()) {
        SDL_RenderClear(mRenderer);

        // NOTE
        // It wan't deallocate pixels, only surface object
        mFbSurface.reset();
        
        SDL_UnlockTexture(mFbTexture.get());
        if(SDL_RenderCopy(mRenderer, m_fbTexture.get(), NULL, NULL)) {
            std::cerr << "SDL_RenderCopy failed: "
                      << SDL_GetError()
                      << std::endl;
        }
    }

    if(!mTextOverlay.empty()) {
        for(TextBatch batch : mTextOverlay) {
            batch();
        }
        mTextOverlay.clear();
    }
    
    SDL_RenderPresent(mRenderer);
}

SDL_Rect RendererImpl::GetOutputSize() const
{
    int width;
    int height;
    if(SDL_GetRendererOutputSize(mRenderer, &width, &height)) {
        std::cerr << "SDL_GetRendererOutputSize failed: "
                  << SDL_GetError()
                  << std::endl;
    }
    return MakeRect(width, height);
}

void RendererImpl::AdjustBufferSize(int width, int height)
{
    if(!mFbSurface.Null()) {
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
    auto cached = mTgxCache.find(filename);
    if(cached != mTgxCache.end()) {
        return cached->second;
    } else {
        Surface loaded = LoadSurface(filename);
        mTgxCache.insert({filename, loaded});
        return loaded;
    }
}

const CollectionData &RendererImpl::QueryCollection(const FilePath &filename)
{
    auto searchResult = mGm1Cache.find(filename);
    if(searchResult != mGm1Cache.end()) {
        return *searchResult->second;
    } else {
        CollectionDataPtr ptr =
            std::move(
                LoadCollectionData(filename));

        if(!ptr)
            throw std::runtime_error("Unable to load collection");
        
        const CollectionData &data = *ptr;
        mGm1Cache.insert(
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
            if(!mTextRenderer->CacheFont(info.name, fontSize, font)) {
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
        mTextRenderer->SetFont(fontname, size);
    };
    mTextOverlay.push_back(changeFont);
}

void RendererImpl::SetColor(const SDL_Color &color)
{
    auto changeColor = [color, this]() {
        mTextRenderer->SetColor(color);
    };
    mTextOverlay.push_back(changeColor);
}

void RendererImpl::RenderTextLine(const std::string &text, const SDL_Point &point)
{
    auto drawText = [text, point, this]() {
        SDL_Rect textRect = mTextRenderer->CalculateTextRect(text);
    
        mTextRenderer->SetCursor(
            ShiftPoint(point, 0, textRect.h));
    
        mTextRenderer->PutString(text);
    };
    
    mTextOverlay.push_back(drawText);
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
