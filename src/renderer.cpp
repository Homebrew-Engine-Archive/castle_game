#include "renderer.h"

typedef std::function<void()> TextBatch;

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
    std::map<std::string, Surface> tgxCache;
    std::map<std::string, CollectionDataPtr> gm1Cache;

    RendererPimpl(SDL_Renderer *renderer);
    
    bool ReallocationRequired(int width, int heigth);
    bool CreateFrameTexture(int width, int height);
    bool CreateFrameSurface(void *pixels, int width, int height, int pitch);
};

RendererPimpl::RendererPimpl(SDL_Renderer *renderer_)
    : renderer {renderer_}
    , textRenderer {renderer_}
    , fbWidth {0}
    , fbHeight {0}
    , fbFormat {0}
{ }

bool RendererPimpl::CreateFrameTexture(int width, int height)
{
    std::clog << "Allocating frame texture: " << std::endl
              << std::dec
              << "\tWidth = " << width << std::endl
              << "\tHeight = " << height << std::endl;
    
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
    const int threshold = 1;
    return ((width / threshold) != (fbWidth / threshold))
        || ((height / threshold) != (fbHeight / threshold));
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
        std::clog << "Discard text overlay"
                  << std::endl;
        m->textOverlay.clear();
    }
    
    if(!m->fbTexture) {
        if(!m->CreateFrameTexture(m->fbWidth, m->fbHeight)) {
            std::cerr << "Can't allocate frame texture"
                      << std::endl;
            return Surface();
        }
    }
    
    if(!m->fbSurface.Null()) {
        std::clog << "Nested BeginFrame" << std::endl;
        return m->fbSurface;
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
        
        m->fbSurface.reset();
        SDL_UnlockTexture(m->fbTexture.get());
        if(SDL_RenderCopy(m->renderer, m->fbTexture.get(), NULL, NULL)) {
            std::cerr << "SDL_RenderCopy failed: "
                      << SDL_GetError()
                      << std::endl;
        }
    }

    if(!m->textOverlay.empty()) {
        for(const auto &batch : m->textOverlay) {
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
    if(m->ReallocationRequired(width, height)) {
        m->CreateFrameTexture(width, height);
    }
}

Surface Renderer::QuerySurface(const std::string &filename)
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

const CollectionData &Renderer::QueryCollection(const std::string &filename)
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

bool Renderer::CacheCollection(const std::string &filename)
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

void EnumRenderDrivers()
{
    int num = SDL_GetNumRenderDrivers();

    std::clog << "Drivers avialable: "
              << std::dec << num
              << std::endl;

    for(int index = 0; index < num; ++index) {
        SDL_RendererInfo info;
        std::clog << "Driver with index: "
                  << std::dec
                  << index
                  << std::endl;
        if(SDL_GetRenderDriverInfo(index, &info)) {
            std::clog << "Can't query driver info"
                      << std::endl;
        } else {
            std::clog << info;
        }
    }
}

void PrintRendererInfo(SDL_Renderer *renderer)
{
    if(renderer != NULL) {
        std::clog << "Renderer info: " << std::endl;
        SDL_RendererInfo info;
        if(SDL_GetRendererInfo(renderer, &info)) {
            std::clog << "\tCan't query renderer info" << std::endl;
        } else {
            std::clog << info;
        }
    }
}
